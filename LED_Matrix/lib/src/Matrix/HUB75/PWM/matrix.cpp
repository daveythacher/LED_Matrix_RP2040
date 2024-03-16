/* 
 * File:   matrix.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#include <stdint.h>
#include <string.h>
#include "pico/platform.h"
#include "hardware/pio.h"
#include "hardware/gpio.h"
#include "hardware/dma.h"
#include "hardware/timer.h"
#include "hardware/structs/bus_ctrl.h"
#include "Matrix/config.h"
#include "Matrix/matrix.h"
#include "Matrix/HUB75/PWM/memory_format.h"
#include "Multiplex/Multiplex.h"
#include "Serial/config.h"

namespace Matrix::Worker {
    extern test2 buf[Serial::num_framebuffers];
}

namespace Matrix {
    static test2 *buffer = nullptr;
    static volatile uint8_t state = 0;
    static int dma_chan[2];
    static volatile struct {volatile uint32_t len; volatile uint8_t *data;} address_table[(1 << PWM_bits) + 2];
    static volatile uint8_t null_table[COLUMNS + 1];
    volatile int timer;

    static void send_line();
    static void load_line(uint32_t rows);

    /*
        There are 2^PWM_bits + 1 shifts per period.
        The last shift turns the columns off before multiplexing.

        The serial protocol used by PIO is column length decremented by one followed by column values.
    */

    void start() {
        // Init Matrix hardware
        // IO
        for (int i = 0; i < 8; i++) {
            gpio_init(i + 8);
            gpio_set_dir(i + 8, GPIO_OUT);
        }
        for (int i = 0; i < 8; i++)
            gpio_set_function(i + 8, GPIO_FUNC_PIO0);
        gpio_init(22);
        gpio_set_dir(22, GPIO_OUT);
        gpio_clr_mask(0x40FF00);

        Multiplex::init(MULTIPLEX);
        
        memset((void *) Worker::buf, COLUMNS - 1, sizeof(Worker::buf));
        memset((void *) null_table, 0, COLUMNS + 1);
        null_table[0] = COLUMNS - 1;

        for (uint32_t i = 0; i < (1 << PWM_bits); i++)
            address_table[i].len = COLUMNS + 1;

        address_table[1 << PWM_bits].data = null_table;
        address_table[1 << PWM_bits].len = COLUMNS + 1;
        address_table[(1 << PWM_bits) + 1].data = NULL;
        address_table[(1 << PWM_bits) + 1].len = 0;
        
        // Hack to lower the ISR tick rate, accelerates by 2^PWM_bits (Improves refresh performance)
        //  Automates CLK and LAT signals with DMA and PIO to handle Software PWM of entire row
        //      Works like Hardware PWM without the high refresh
        //      This is more or less how it would work with MACHXO2 FPGA and PIC32MX using PMP.
        //          Bus performance is better with RP2040. (Lower cost due to memory, CPU, hardware integration.)
        //  OE is not used in this implementation and held to low to enable the display
        //      Last shift will disable display.
        /*while (1) {
            counter2 = (1 << PWM_bits) - 1; LAT = 0;    // Start of frame, manually push into FIFO (data stream protocol)
            do {
                counter = COLUMNS - 1;                  // Start of payload, DMA push into FIFO (data stream protocol)
                do {
                    DAT = DATA; CLK = 0;                // Payload data, DMA push into FIFO (data stream protocol)
                    CLK = 1;                            // Automate CLK pulse
                } while (counter-- > 0); CLK = 0;
                LAT = 1;                                // Automate LAT pulse at end of payload (bitplane shift)
                LAT = 0;
            } while (counter2-- > 0);
        }*/
    
        // PIO
        const uint16_t instructions[] = {
            (uint16_t) (pio_encode_pull(false, true) | pio_encode_sideset(2, 0)),   // PIO SM
            (uint16_t) (pio_encode_out(pio_x, 8) | pio_encode_sideset(2, 0)),
            (uint16_t) (pio_encode_out(pio_y, 8) | pio_encode_sideset(2, 0)),
            (uint16_t) (pio_encode_out(pio_pins, 6) | pio_encode_sideset(2, 0)),    // PMP Program
            (uint16_t) (pio_encode_jmp_y_dec(3) | pio_encode_sideset(2, 1)),
            (uint16_t) (pio_encode_nop() | pio_encode_sideset(2, 2)),
            (uint16_t) (pio_encode_nop() | pio_encode_sideset(2, 2)),
            (uint16_t) (pio_encode_nop() | pio_encode_sideset(2, 0)),
            (uint16_t) (pio_encode_jmp_x_dec(2) | pio_encode_sideset(2, 0)),
            (uint16_t) (pio_encode_jmp(0) | pio_encode_sideset(2, 0))
        };
        static const struct pio_program pio_programs = {
            .instructions = instructions,
            .length = count_of(instructions),
            .origin = 0,
        };
        pio_add_program(pio0, &pio_programs);
        pio_sm_set_consecutive_pindirs(pio0, 0, 8, 8, true);
        
        // Verify Serial Clock
        constexpr float x = 125000000.0 / (SERIAL_CLOCK * 2.0);
        static_assert(x >= 1.0, "Unabled to configure PIO for SERIAL_CLOCK");

        Calculator::verify_configuration();

        // PMP / SM
        pio0->sm[0].clkdiv = ((uint32_t) floor(x) << 16) | ((uint32_t) round((x - floor(x)) * 255.0) << 8);
        pio0->sm[0].pinctrl = (2 << PIO_SM0_PINCTRL_SIDESET_COUNT_LSB) | (6 << PIO_SM0_PINCTRL_OUT_COUNT_LSB) | (14 << PIO_SM0_PINCTRL_SIDESET_BASE_LSB) | 8;
        pio0->sm[0].shiftctrl = (1 << PIO_SM0_SHIFTCTRL_AUTOPULL_LSB) | (6 << 25) | (1 << 19);
        pio0->sm[0].execctrl = (1 << 17) | (12 << 12);
        pio0->sm[0].instr = pio_encode_jmp(0);
        hw_set_bits(&pio0->ctrl, 1 << PIO_CTRL_SM_ENABLE_LSB);
        pio_sm_claim(pio0, 0);
        
        // DMA
        bus_ctrl_hw->priority = (1 << 12) | (1 << 8);
        dma_chan[0] = dma_claim_unused_channel(true);
        dma_chan[1] = dma_claim_unused_channel(true);
        dma_channel_config c = dma_channel_get_default_config(dma_chan[0]);
        channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
        channel_config_set_read_increment(&c, true);
        channel_config_set_dreq(&c, DREQ_PIO0_TX0);
        channel_config_set_chain_to(&c, dma_chan[1]);
        channel_config_set_irq_quiet(&c, true);
        dma_channel_configure(dma_chan[0], &c, &pio0_hw->txf[0], NULL, 0, false);
        dma_channel_set_irq0_enabled(dma_chan[0], true); 
        
        c = dma_channel_get_default_config(dma_chan[1]);
        channel_config_set_transfer_data_size(&c, DMA_SIZE_32);
        channel_config_set_read_increment(&c, true);
        channel_config_set_write_increment(&c, true);
        channel_config_set_ring(&c, true, 3);                                       // 1 << 3 byte boundary on write ptr
        dma_channel_configure(dma_chan[1], &c, &dma_hw->ch[dma_chan[0]].al3_transfer_count, &address_table[0], 2, false);

        timer = hardware_alarm_claim_unused(true);
        timer_hw->inte |= 1 << timer;
        
        load_line(0);
        send_line();
    }

    void __not_in_flash_func(send_line)() {
        dma_hw->ints0 = 1 << dma_chan[0];
        pio_sm_put(pio0, 0, 1 << PWM_bits);
        dma_channel_set_read_addr(dma_chan[1], &address_table[0], true);
    }

    // This is done to reduce interrupt rate. Use DMA to automate the PWM bitplanes instead of CPU.
    //  This is possible due to PIO state machine.
    void __not_in_flash_func(load_line)(uint32_t rows) {
        for (uint32_t i = 0; i < (1 << PWM_bits); i++)
                address_table[i].data = (*buffer)[rows][i];
    }

    static void __not_in_flash_func(dma_isr)() {
        if (dma_channel_get_irq0_status(dma_chan[0])) {      
            // Make sure the FIFO is empty 
            //  There was a bug with LAT when SERIAL_CLOCK is small, but I forgot what it was. (Just counted out additional time as hack.)
            constexpr uint32_t FIFO_delay = (uint32_t) 125000000U / ((uint32_t) round(SERIAL_CLOCK) / 5 * 1000);
            timer_hw->alarm[timer] = time_us_32() + FIFO_delay + 1;                 // Load timer
            timer_hw->armed = 1 << timer;                                           // Kick off timer
            state = 0;
            dma_hw->intr = 1 << dma_chan[0];                                        // Clear the interrupt
        }
    }

    static void __not_in_flash_func(timer_isr)() {
        static uint32_t rows = 0;

        if (timer_hw->ints & (1 << timer)) {                                        // Verify who called this
            switch(state) {
                case 0:
                    gpio_set_mask(1 << 22);                                                 // Turn off the panel (For MBI5124 this activates the low side anti-ghosting)
                    timer_hw->alarm[timer] = time_us_32() + BLANK_TIME + 1;                 // Load timer (We don't care if it rolls over!)
                    timer_hw->armed = 1 << timer;                                           // Kick off timer
                    
                    if (++rows >= MULTIPLEX) {                                              // Fire rate: MULTIPLEX * REFRESH (Note we now call 3 ISRs per fire)
                        rows = 0;

                        do {
                            test2 *p;

                            if (Serial::isPacket)
                                p = (test2 *) Worker::get_front_buffer();
                            else
                                p = (test2 *) Loafer::get_front_buffer();

                            if (p != nullptr)
                                buffer = p;
                        } while (buffer == nullptr);
                    }

                    Multiplex::SetRow(rows);
                    load_line(rows);                                                        // Note this is a fairly expensive operation. This is done in parallel with blank time.
                    state++;
                    break;
                case 1:
                    gpio_clr_mask(1 << 22);                                         // Turn on the panel (Note software controls PWM/BCM)
                    send_line();                                                    // Kick off hardware
                    state++;
                    break;
                default:
                    break;
            }
            
            timer_hw->intr = 1 << timer;                                            // Clear the interrupt
        }
    }

    void __not_in_flash_func(loop)() {
        timer_isr();
        dma_isr();
    }
}

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
#include "Matrix/HUB75/BCM/memory_format.h"
#include "Multiplex/Multiplex.h"
#include "Serial/config.h"
#include "Matrix/HUB75/hw_config.h"

namespace Matrix {
    static int dma_chan[2];
    static uint8_t bank = 0;
    static bool hold = false;
    volatile int timer;     // TODO: Remove

    // PIO Protocol
    //  There are 2^PWM_bits shifts per period.
    //      The serial protocol used by PIO is column length decremented by one followed by column values.
    //          The PIO logic is indexed from zero, and there is no way to command zero transfer length.
    //      The serial protocol has a header which indicates the number of transfers to expect
    //          This is loaded by the CPU manually before starting DMA (excludes null termination transfer)
    //  There are 2^PWM_bits plus two transfers.
    //      The second to last transfer turns the columns off before multiplexing. (Standard shift)
    //      The last transfer stops the DMA and fires an interrupt.
    static volatile struct {volatile uint32_t len; volatile uint8_t *data;} address_table[2][MULTIPLEX + 1][(1 << PWM_bits) + 1];   // TODO: Clean up with 3-D DMA table
    static volatile uint8_t null_table[COLUMNS + 2];

    static void send_buffer();
    static void load_buffer(Buffer *buffer);

    void start() {
        // Init Matrix hardware
        // IO
        for (int i = 0; i < Matrix::HUB75::HUB75_DATA_LEN; i++) {
            gpio_init(i + Matrix::HUB75::HUB75_DATA_BASE);
            gpio_set_dir(i + Matrix::HUB75::HUB75_DATA_BASE, GPIO_OUT);
            gpio_set_function(i + Matrix::HUB75::HUB75_DATA_BASE, GPIO_FUNC_PIO0);
        }
        gpio_init(Matrix::HUB75::HUB75_OE);
        gpio_set_dir(Matrix::HUB75::HUB75_OE, GPIO_OUT);
        gpio_clr_mask(0x40FF00);

        Multiplex::init(MULTIPLEX); // TODO: Convert this to PIO program
       
        memset((void *) null_table, 0, COLUMNS + 1);
        null_table[0] = COLUMNS - 1;
        null_table[COLUMNS + 1] = (2 * SERIAL_CLOCK) / (1000000 / BLANK_TIME);

        for (uint32_t b = 0; b < 2; b++) {
            for (uint32_t m = 0; m < MULTIPLEX; m++) {
                for (uint32_t i = 0; i < (1 << PWM_bits); i++) {
                    address_table[b][m][i].len = Buffer::get_line_length();
                }
                
                address_table[b][m][1 << PWM_bits].data = null_table;
                address_table[b][m][1 << PWM_bits].len = COLUMNS + 2;
            }

            address_table[b][MULTIPLEX][0].data = NULL;
            address_table[b][MULTIPLEX][0].len = 0;
        }
        
        // Hack to lower the ISR tick rate, accelerates by 2^PWM_bits (Improves refresh performance)
        //  Automates CLK and LAT signals with DMA and PIO to handle Software PWM of entire row
        //      Works like Hardware PWM without the high refresh
        //      This is more or less how it would work with MACHXO2 FPGA and PIC32MX using PMP.
        //          Bus performance is better with RP2040. (Lower cost due to memory, CPU, hardware integration.)
        //  OE is not used in this implementation and held to low to enable the display
        //      Last shift will disable display.
        //
        //  while (1) {
        //      counter2 = (1 << PWM_bits) - 1; LAT = 0;    // Start of frame, manually push into FIFO (data stream protocol)
        //      do {
        //          counter = COLUMNS - 1;                  // Start of payload, DMA push into FIFO (data stream protocol)
        //          do {
        //              DAT = DATA; CLK = 0;                // Payload data, DMA push into FIFO (data stream protocol)
        //              CLK = 1;                            // Automate CLK pulse
        //          } while (counter-- > 0); CLK = 0;
        //          LAT = 1;                                // Automate LAT pulse at end of payload (bitplane shift)
        //          LAT = 0;
        //      } while (counter2-- > 0);
        //  }
        
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
            // TODO: Kill the madness that blocks memmory and wastes time
            (uint16_t) (pio_encode_jmp(0) | pio_encode_sideset(2, 0))
        };
        static const struct pio_program pio_programs = {
            .instructions = instructions,
            .length = count_of(instructions),
            .origin = 0,
        };

        // TODO: Add OE program

        pio_add_program(pio0, &pio_programs);
        pio_sm_set_consecutive_pindirs(pio0, 0, Matrix::HUB75::HUB75_DATA_BASE, Matrix::HUB75::HUB75_DATA_LEN, true);
        
        // Verify Serial Clock
        constexpr float x = 125000000.0 / (SERIAL_CLOCK * 2.0);     // Someday this two will be a four.
        static_assert(x >= 1.0, "Unabled to configure PIO for SERIAL_CLOCK");

        Calculator::verify_configuration();

        // PMP / SM
        pio0->sm[0].clkdiv = ((uint32_t) floor(x) << PIO_SM0_CLKDIV_INT_LSB) | ((uint32_t) round((x - floor(x)) * 255.0) << PIO_SM0_CLKDIV_FRAC_LSB);
        pio0->sm[0].pinctrl = (2 << PIO_SM0_PINCTRL_SIDESET_COUNT_LSB) | (6 << PIO_SM0_PINCTRL_OUT_COUNT_LSB) | (14 << PIO_SM0_PINCTRL_SIDESET_BASE_LSB) | (Matrix::HUB75::HUB75_DATA_BASE << PIO_SM0_PINCTRL_OUT_BASE_LSB);
        pio0->sm[0].shiftctrl = (1 << PIO_SM0_SHIFTCTRL_AUTOPULL_LSB) | (6 << PIO_SM0_SHIFTCTRL_PULL_THRESH_LSB) | (1 << PIO_SM0_SHIFTCTRL_OUT_SHIFTDIR_LSB);
        pio0->sm[0].execctrl = (1 << PIO_SM1_EXECCTRL_OUT_STICKY_LSB) | (12 << PIO_SM1_EXECCTRL_WRAP_TOP_LSB);
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
        channel_config_set_high_priority(&c, true);
        channel_config_set_dreq(&c, DREQ_PIO0_TX0);
        channel_config_set_chain_to(&c, dma_chan[1]);
        channel_config_set_irq_quiet(&c, true);
        dma_channel_configure(dma_chan[0], &c, &pio0_hw->txf[0], NULL, 0, false);
        dma_channel_set_irq0_enabled(dma_chan[0], true); 
        
        c = dma_channel_get_default_config(dma_chan[1]);
        channel_config_set_transfer_data_size(&c, DMA_SIZE_32);
        channel_config_set_read_increment(&c, true);
        channel_config_set_write_increment(&c, true);
        channel_config_set_high_priority(&c, true);
        channel_config_set_ring(&c, true, 3);                                       // 1 << 3 byte boundary on write ptr
        dma_channel_configure(dma_chan[1], &c, &dma_hw->ch[dma_chan[0]].al3_transfer_count, &address_table[0], 2, false);

        timer = hardware_alarm_claim_unused(true);
        timer_hw->inte |= 1 << timer;

        Buffer *buffer = nullptr;

        do {
            buffer = Worker::get_front_buffer();
        } while (buffer == nullptr);
        
        load_buffer(buffer);
        bank = (bank + 1) % 2;
        send_buffer();
    }

    void __not_in_flash_func(send_buffer)() {
        dma_hw->ints0 = 1 << dma_chan[0];
        pio_sm_put(pio0, 0, (1 << PWM_bits) - 1); // TODO: Fix this
        dma_channel_set_read_addr(dma_chan[1], &address_table[bank][0][0], true);
    }

    // This is done to reduce interrupt rate. Use DMA to automate the BCM bitplanes instead of CPU.
    //  This is possible due to PIO state machine.
    void __not_in_flash_func(load_buffer)(Buffer *buffer) {
        for (uint32_t rows = 0; rows < MULTIPLEX; rows++) {
            for (uint32_t i = 0; i < PWM_bits; i++) {
                for (uint32_t k = 0; k < (uint32_t) (1 << i); k++) {
                    address_table[(bank + 1) % 2][rows][(1 << i) + k - 1].data = buffer->get_line(rows, i);
                }
            }
        }

        hold = true;
    }

    void __not_in_flash_func(matrix_task)(void) {
        if (!hold) {
            Buffer *p = Worker::get_front_buffer();

            if (p != nullptr) {
                load_buffer(p);                                                     // Capable of killing the thread!
            }
        }

        if (dma_channel_get_irq0_status(dma_chan[0])) {                             // We may have the tick soft enough to run via preemptive OS
            if (hold) {
                bank = (bank + 1) % 2;
                hold = false;
            }

            send_buffer();
        }
    }

    void __not_in_flash_func(dma_isr)() {
        // TODO: Remove
    }

    void __not_in_flash_func(timer_isr)() {
        // TODO: Remove
    }
}

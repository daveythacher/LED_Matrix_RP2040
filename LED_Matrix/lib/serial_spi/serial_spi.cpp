/* 
 * File:   serial_spi.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#include <stdint.h>
#include <assert.h>
#include "hardware/gpio.h"
#include "hardware/dma.h"
#include "serial_spi/serial_spi.h"

static serial_spi_callback func;
static int dma_chan;
static PIO pio;
static int pio_sm;
static dma_channel_config c;

static void serial_spi_reload();

void serial_spi_start(serial_spi_callback callback, int dma, PIO p, int sm) {
    func = callback;
    dma_chan = dma;
    pio_sm = sm;
    pio = p;
    
    // IO
    for (int i = 0; i < 3; i++) {
        gpio_init(i + 16);
        
        if (pio == pio0)
            gpio_set_function(i + 16, GPIO_FUNC_PIO0);
        else
            gpio_set_function(i + 16, GPIO_FUNC_PIO1);
    }
    
    /* Max clock is 125MHz / 6 = 20.83MHz
        while(!SCLK);
        if (!CS)
            data = (data << 1) + SDO;
        while(SCLK);
    */
    
    // PIO
    uint16_t instructions[] = {
        (uint16_t) (pio_encode_wait_pin(1, 1)),     // SPI
        (uint16_t) (pio_encode_jmp_pin(3)),
        (uint16_t) (pio_encode_in(pio_pins, 1)),
        (uint16_t) (pio_encode_wait_pin(0, 1))
    };
    static const struct pio_program pio_programs = {
        .instructions = instructions,
        .length = count_of(instructions),
        .origin = 0,
    };
    
    int i;
    for (i = 0; i < 32; i++) {
        if (pio_can_add_program_at_offset(p, &pio_programs, i)) {
            instructions[1] = (uint16_t) (pio_encode_jmp_pin((3 + i) % 32));
            pio_add_program_at_offset(p, &pio_programs, i);
            break;
        }
    }
    assert(i < 32);
    pio_sm_set_consecutive_pindirs(p, pio_sm, 16, 3, false);

    //  SPI
    p->sm[pio_sm].clkdiv = (1 << 16) | (0 << 8);
    p->sm[pio_sm].pinctrl = (16 << 15);
    p->sm[pio_sm].shiftctrl = (1 << 16) | (16 << 20) | (1 << 18);
    p->sm[pio_sm].execctrl = (18 << 24) | (((4 + i) % 32) << 12) | ((0 + i) << 7);
    p->sm[pio_sm].instr = pio_encode_jmp(i);
    hw_set_bits(&p->ctrl, 1 << (PIO_CTRL_SM_ENABLE_LSB + pio_sm));
    
    // DMA
    c = dma_channel_get_default_config(dma_chan);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_16);
    channel_config_set_write_increment(&c, true);
    channel_config_set_read_increment(&c, false);
    if (pio == pio0)
        channel_config_set_dreq(&c, DREQ_PIO0_RX0 + pio_sm);
    else
        channel_config_set_dreq(&c, DREQ_PIO1_RX0 + pio_sm);
    dma_channel_set_irq0_enabled(dma_chan, true); 
    
    serial_spi_reload();
}

// Warning: Weak protocol/state machine!
void serial_spi_reload() {
    static uint8_t *buf;
    static uint16_t len;
    static bool recovering = false;
    
    if (dma_channel_get_irq0_status(dma_chan)) {
        uint32_t *ptr = (uint32_t *) buf;                                       // Warning: Endiness is assumed to be little!
        
        if (*ptr == 0x20131990 && *(ptr + 1) == ~*ptr) {                        // Note: 1 in 2^64 chance of failing to detect error
            if (!recovering)
                func(&buf, &len);  
              
            if (pio == pio0)
                dma_channel_configure(dma_chan, &c, buf, &pio0_hw->txf[pio_sm], len / 2, true);
            else
                dma_channel_configure(dma_chan, &c, buf, &pio1_hw->txf[pio_sm], len / 2, true);
            recovering = false;
        }
        else {                                                                  // Warning: Has high event rate. (20.83MHz / 64 = 325.5kHz)
            if (pio == pio0)
                dma_channel_configure(dma_chan, &c, buf, &pio0_hw->txf[pio_sm], 4, true);
            else
                dma_channel_configure(dma_chan, &c, buf, &pio1_hw->txf[pio_sm], 4, true);
            recovering = true;
        }
    }
}

void serial_spi_isr() {
    serial_spi_reload();
    dma_hw->ints0 = 1 << dma_chan;
}


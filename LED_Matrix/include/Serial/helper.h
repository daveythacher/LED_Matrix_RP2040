/* 
 * File:   helper.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef SERIAL_HELPER_H
#define SERIAL_HELPER_H

#include <hardware/pio.h>

namespace APP {
    // Detect leading edge of trigger and assert ISR
    void setup_debouncer(PIO pio, int sm, int pin) {
        // uint32_t osr = pull_fifo();
        // uint32_t isr_flag = 0;
        // while (true) {
        //      uint32_t x = osr;
        //      while (true) {
        //          if (pin) {
        //              if (x--)
        //                  continue;
        //              else {
        //                  isr_flag = 1;
        //                  while (isr_flag);
        //                  break;
        //              }
        //          }
        //          else
        //              break;
        //      }
        // }
        const uint16_t instructions[] = {
            // uint32_t osr = pull_fifo();
            (uint16_t) pio_encode_pull(false, true),            // Line 0

            // uint32_t isr_flag = 0;
            (uint16_t) pio_encode_irq_clear(false, sm),          // Line 1

            // while (true) {
            //      uint32_t x = osr;
            (uint16_t) pio_encode_mov(pio_x, pio_osr),          // Line 2

            //      while (true) {
            //          if (pin)
            (uint16_t) pio_encode_jmp_pin(5),                   // Line 3

            //          else
            //              break;
            (uint16_t) pio_encode_jmp(2),                       // Line 4

            //              if (x--)
            (uint16_t) pio_encode_jmp_x_dec(9),                 // Line 5

            //                  isr_flag = 1;
            (uint16_t) pio_encode_irq_set(false, sm),            // Line 6

            //                  while (isr_flag);
            (uint16_t) pio_encode_wait_irq(false, false, sm),    // Line 7

            //                  break;
            (uint16_t) pio_encode_jmp(2),                       // Line 8

            //                  continue;
            (uint16_t) pio_encode_jmp(3),                       // Line 9
        };

        static const struct pio_program pio_programs = {
            .instructions = instructions,
            .length = count_of(instructions),
            .origin = 0,
        };

        pio_add_program(pio, &pio_programs);
        pio_sm_set_consecutive_pindirs(pio, sm, pin, 1, false);

        // TODO:
    }
}

#endif


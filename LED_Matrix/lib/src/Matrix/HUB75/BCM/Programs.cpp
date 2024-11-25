#include <algorithm>
#include "Matrix/HUB75/BCM/Programs.h"
#include "PIO/Program.h"
#include "PIO/ASM/NOP/NOP.h"
#include "PIO/ASM/OUT/OUT.h"
#include "PIO/ASM/PULL/PULL.h"
#include "PIO/ASM/JMP/JMP.h"
#include "PIO/ASM/IRQ/IRQ.h"
using namespace PIO;

namespace Matrix {
    const auto X = Registers::X;
    const auto Y = Registers::Y;
    const auto PINS = Registers::PINS;
    const auto X_DEC = Conditions::X_DEC;
    const auto Y_DEC = Conditions::Y_DEC;
    
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
    uint8_t Programs::get_pmp_program(uint16_t *instructions, uint8_t len) {
        uint8_t CLK = 1 << 0;
        uint8_t LAT = 1 << 1;
        uint8_t size;
        Program PMP(2);     // Warning not all behavior is supported
        ASM program[] = {   // Sidesets are cleared by default
            PULL(true),
            OUT(X, 8),
            OUT(Y, 8),
            OUT(PINS, 6),
            JMP(Y_DEC, 3).sideset(CLK),
            NOP().sideset(LAT),
            NOP().sideset(LAT),
            NOP(),
            JMP(X_DEC, 2),
            IRQ(true, WAKE_GHOST),
            IRQ(false, WAKE_PMP),
            JMP(0)
        };

        size = sizeof(program) / sizeof(ASM);
        size = std::min(len, size);
        PMP.replace(program, 0, size);
        PMP.translate(0, instructions, size);
        return size;
        
        // PIO
        /*const uint16_t instructions[] = {
            (uint16_t) (pio_encode_pull(false, true) | pio_encode_sideset(2, 0)),   // PIO SM
            (uint16_t) (pio_encode_out(pio_x, 8) | pio_encode_sideset(2, 0)),
            (uint16_t) (pio_encode_out(pio_y, 8) | pio_encode_sideset(2, 0)),
            (uint16_t) (pio_encode_out(pio_pins, 6) | pio_encode_sideset(2, 0)),    // PMP Program
            (uint16_t) (pio_encode_jmp_y_dec(3) | pio_encode_sideset(2, 1)),
            (uint16_t) (pio_encode_nop() | pio_encode_sideset(2, 2)),
            (uint16_t) (pio_encode_nop() | pio_encode_sideset(2, 2)),
            (uint16_t) (pio_encode_nop() | pio_encode_sideset(2, 0)),
            (uint16_t) (pio_encode_jmp_x_dec(2) | pio_encode_sideset(2, 0)),
            // Raise the signal to the ghosting program
            //  Ghost load scan register
            //  Ghost wait for PMP program
            //  Ghost turn off OE
            //  Ghost call address program
            //      Address pull and write
            //      Address wrap
            //  Ghost delay for blank delay
            //  Ghost wait for address program
            //  Ghost turn on OE
            //  Ghost return to loop
            //  Ghost call CPU, if scan == 0
            //  Ghost wrap
            // Wait for ghost program
            (uint16_t) (pio_encode_jmp(0) | pio_encode_sideset(2, 0))
        };*/
    }

    uint8_t Programs::get_ghost_program(uint16_t *instructions, uint8_t len) {
        uint8_t OE = 1 << 0;
        uint8_t size;
        Program PMP(2);     // Warning not all behavior is supported
        ASM program[] = {   // Sidesets are cleared by default
            PULL(true).sideset(OE),
            OUT(X, 8).sideset(OE),
            IRQ(false, WAKE_GHOST).sideset(OE),
            IRQ(true, WAKE_MULTIPLEX),
            PULL(true),
            OUT(Y, 8),
            NOP(),
            JMP(Y_DEC, 7),
            IRQ(false, WAKE_GHOST).sideset(OE),
            JMP(X_DEC, 2).sideset(OE),
            IRQ(true, WAKE_PMP).sideset(OE),
            JMP(0).sideset(OE)
        };

        size = sizeof(program) / sizeof(ASM);
        size = std::min(len, size);
        PMP.replace(program, 0, size);
        PMP.translate(0, instructions, size);
        return size;        
    }

    uint8_t Programs::get_address_program(uint16_t *instructions, uint8_t len) {
        uint8_t size;
        Program PMP(2);     // Warning not all behavior is supported
        ASM program[] = {   // Sidesets are cleared by default
            PULL(true),
            IRQ(false, WAKE_MULTIPLEX),
            OUT(PINS, 5),
            IRQ(true, WAKE_GHOST),
            JMP(0)
        };

        size = sizeof(program) / sizeof(ASM);
        size = std::min(len, size);
        PMP.replace(program, 0, size);
        PMP.translate(0, instructions, size);
        return size;
    }
}
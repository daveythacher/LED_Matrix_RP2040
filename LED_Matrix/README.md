# Internal Documentation
Place to talk about internal code stuff

Critical sections of code are RAM functions, as I do not trust XIP cache for this. Be mindful of interrupts. Currently multiplexing generates a lot of interrupts, which can cause issues. Be mindful of interrupt allocation, which can also cause issues.

## Overview
This code base is divided into two parts. The first part is applications which are front end protocols. The second part is matrix algorithms which are back end panel implementations. Not all features are implemented in this code base. For example: gamma, brightness, CIE1931, pixel mapping, etc. do not belong in this code base these are handled by the application logic. This code base allows the application logic to change the order of the pixels in the RGB pixel buffer which is sent over the front end protocol.

## lib folder
This is a folder for different libraries which could be used by multiple applications.

### Matrix Alogirthms
I did not implement these as C++ classes. The memory footprints vary and currently are defined by the preprocessor. I do not want to support dynamic memory as this can be a pain. I also do not want to use union for the different algorithms of matrix. This forced me to use static and only support one at a time.

Currently they all use the same symbol, matrix_start to get going. They are compiled as library: led_BCM, etc. You must link against one of these. Note ISR logic is handled in application implementation. There is one callbacks allocated to matrix algorithms, matrix_dma_isr. 

To add more you just create folder, define matrix_start, have CMake build a library with led_ prefix, define matrix_dma_isr if needed, define set_pixel, etc. CMake should link against all dependencies used by the lib. Each matrix algorithm is its own library.

Parallel development should be possible.

### Multiplex Alogirthms
CMake configuration selects these as configured for the specific build. Parallel development should be possible.

### Serial Algorithms
These are used by the src folder to implement different applications/front end protocols. Configuration for specific build picks the src folder implementation. Parallel development should be possible.

Note these have control over ISRs. These are also responsible for booting core 1 which is owned by matrix algorithm. Design of serial protocols is a decent topic depending on application.

#### serial_uart
This attempts to use a flow control scheme with real time data. Packets are sent as continuous segment. There is a checksum to ensure the packet is complete. If packet is incomplete or incorrect it is discarded and acknowledged as received. If the packet comes too fast and the packet queue is full, the packet is discarded and acknowledged as received.

This is recommended for most protocols where possible. This implementation avoided addressing chunks of data or micro-packets for simplicity and performance on core 0. Core 0 when configured for high refresh rates may not support a high packet rate otherwise.

## src folder
This is boiler plate application which pulls the matrix and serial algorithms together. CMake configures preprocessor for size, multiplex algorithm, matrix algorithm, serial algorithm, etc. The idea here is build flavors.

## Benchmark
May be worth correcting jitter using non-stripped RAM. However, this is not believed to be required. DMA to PIO latency would be more critical than jitter or computation latency, but I am not sure there is a significant issue. Manually mapping this will likely increase complexity and reduce available RAM.

### copy_to_ram vs default linker script
Currently copy_to_ram is used as this code base is fairly small and it can fit.

Results from computation latency test on Core 1 without UART serial ISR: (16x64 7 bit)

Increased jitter from 64ns to 289ns, moving from default to copy_to_ram. Increased computation latency from 1.06mS to 1.09mS, moving from default to copy_to_ram. This is believed to be from stripped RAM arbitration.

### __not_in_flash_func
Currently these are only assigned on ISRs, functions called by ISRs and critical functions where latency would cause issues. Bigger lower priority things should not get this, these will fight for space in XIP.

Results from computation latency test on Core 1 without UART serial ISR: (16x64 7 bit)

Increased computation latency from 1.09mS to 1.11mS, when using copy_to_ram. The exact reason for this is not known, but is still believed to be arbitration related. Using default linker script with __not_in_flash_func improves computation latency. This is higher than default linker script without __not_in_flash_func.

Note these results are old.

## Interrupts
Core 0 should run all front end serial logic. Core 1 should run all processing in loop for GEN 1 and all GCLK/multiplexing in loop for GEN 2 and GEN 3. Note GEN 2 and GEN 3 may process in the ISR on core 0, however the DMA is to be activated before if used. Processing in GEN 2 and GEN 3 should be very fast. There is no processing on core 0 outside of GEN 1 multiplexing interrupts or front end serial protocols.

Serial protocol interrupts are the highest priority, but this is discouraged. (It is recommended to use core 0's loop.) Next is multiplexing interrupts, if any. (GEN 1 works this way.) Matrix algorithms are free to configure interrupts on Core 1 if they desire. Matrix algorithms own core 1. Note performance can slip if core 1 is mismanaged. Responsiveness is the most important thing and the multiplexing is to never be compromised under any case.

## Status
### lib/Matrix:
BCM - working

PWM - working

ON_OFF - not tested

TLC5946 - experimental

TLC5958 - experimental

### lib/Multiplex:
Decoder - working

Direct - working

### lib/Serial:
serial_uart - experimental

## Random note
I use a linker trick with C abstract class/functions to avoid complexity in compilation. This is also reduces code size and improves run time performance. It should be possible to do this with C++ using abstract class and templates. This would still require the linker or run time, which made this seem pointless to me. The code expression is currently close enough for templates to not make sense.

This was done in Multiplex, Matrix and Serial. These are three abstract classes/APIs which the C preprocessor and CMake are used to modify compilation and linking. The strings used come from the configuration file directly.

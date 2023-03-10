# Internal Documentation
Place to talk about internal code stuff

Critical sections of code are RAM functions, as I do not trust XIP cache for this. Be mindful of interrupts. Currently multiplexing generates a lot of interrupts, which can cause issues. Be mindful of interrupt allocation, which can also cause issues.

Someday the build logic will need to change away from preprocessor logic. Could use CMake to generate config headers. Most of the current structure should support this. Groovy could generate the configure files for config heres. There will need to be a fake header layer for the library and application logic. CMake will need to manage the locations of these. Lot of work and code that at this point is for nothing really.

## Overview
This code base is divided into two parts. The first part is applications which are front end protocols. The second part is matrix algorithms which are back end panel implementations. Not all features are implemented in this code base. For example: gamma, brightness, CIE1931, pixel mapping, etc. do not belong in this code base these are handled by the application logic. This code base allows the application logic to change the order of the pixels in the RGB pixel buffer which is sent over the front end protocol.

## lib folder
This is a folder for different libraries which could be used by multiple applications.

### Matrix Alogirthms
I did not implement these as C++ classes. The memory footprints vary and currently are defined by the preprocessor. I do not want to support dynamic memory as this can be a pain. I also do not want to use union for the different algorithms of matrix. This forced me to use static and only support one at a time.

Currently they all use the same symbol, matrix_start to get going. They are compiled as library: led_BCM, etc. You must link against one of these. Note ISR logic is handled in application implementation. There is one callbacks allocated to matrix algorithms, matrix_dma_isr. 

To add more you just create folder, define matrix_start, have CMake build a library with led_ prefix, define matrix_dma_isr if needed, define set_pixel, etc. CMake should link against all dependencies used by the lib. Each matrix algorithm is its own library.

### Multiplex Alogirthms
CMake configuration selects these as configured for the specific build.

### Serial Algorithms
These are used by the src folder to implement different applications/front end protocols. Configuration for specific build picks the src folder implementation.

## src folder
This folder for different applications. These pick a serial algorithm. CMake still configures preprocessor for size, multiplex algorithm, matrix algorithm, etc. The idea here is build flavors. Some matrix algorithms require a completely different implementation or cannot work in all cases.

This is kind of pain, but this is a microcontroller. I did not want to lose memory or make this really painful to manage. Parallel development should be possible.

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
Core 0 should run all interrupts. Core 1 should run all processing for GEN 1 and all GCLK interrupts for GEN 2 and GEN 3. Note GEN 2 and GEN 3 process in the ISR, however the DMA is activated before. Processing in GEN 2 and GEN 3 should be very fast. There is no processing on Core 0 outside of interrupts.

Serial protocol interrupts are the highest priority. Next is multiplexing interrupts, if any. Matrix algorithms are free to configure interrupts on Core 1 if they desire. Matrix algorithms own Core 1.

## Status
### lib/Matrix:
BCM - not fully tested, working

PWM - not fully tested, working

ON_OFF - not tested

TLC5946 - experimental

TLC5958 - experimental

### lib/Multiplex:
Decoder - working

Direct - working

### lib/Serial:
serial_pmp - experimental

serial_uart - experimental

serial_uart_v2 - experimental

### src:
pmp - not fully tested

uart - not fully tested

uart_v2 - not fully tested


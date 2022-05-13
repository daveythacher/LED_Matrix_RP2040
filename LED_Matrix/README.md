Place to talk about internal code stuff

Critical sections of code are RAM functions, as I do not trust XIP cache for this. Be mindful of interrupts. Currently multiplexing generates a lot of interrupts, which can cause issues. Be mindful of interrupt allocation, which can also cause issues.

Someday the build logic will need to change away from preprocessor logic. Could use CMake to generate config headers. Most of the current structure should support this. Groovy could generate the configure files for config heres. There will need to be a fake header layer for the library and application logic. CMake will need to manage the locations of these. Lot of work and code that at this point is for nothing really.

## lib folder
This is a folder for different libraries which could be used by multiple applications.

### Matrix Alogirthms
I did not implement these as C++ classes. The memory footprints vary and currently are defined by the preprocessor. I do not want to support dynamic memory as this can be a pain. I also do not want to use union for the different algorithms of matrix. This forced me to use static and only support one at a time.

Currently they all use the same symbol, matrix_start to get going. They are compiled as library: led_BCM, led_SPWM, etc. You must link against one of these. Note ISR logic is handled in application implementation. There are two callbacks allocated to matrix algorithms, matrix_dma_isr and matrix_pio_isr. 

To add more you just create folder, define matrix_start, have CMake build a library with led_ prefix, define matrix_dma_isr and matrix_pio_isr if needed, define set_pixel, etc. CMake should link against all dependencies used by the lib. Each matrix algorithm is its own library.

### Multiplex Alogirthms
I did implement these as C++ class. To add more just extent the interface and add it to the factory in the base class. CMake should link against all dependencies used by the lib. There is only one multiplex library for all multiplex algorithms.

## src folder
This folder for different applications. These pick a matrix algorithm. CMake still configures preprocessor for size, multiplex algorithm, etc. The idea here is build flavors. Some matrix algorithms require a completely different implementation or cannot work in all cases. Rather than do anything smart, I just did this.

This is kind of pain, but this is a microcontroller. I did not want to lose memory or make this really painful to manage. Parallel development should be possible.


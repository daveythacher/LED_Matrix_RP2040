# Since we use preprocessor we have to use interface library
#   Optimization likely destroys any point in making this an actual lib

add_library(led_ON_OFF INTERFACE)

target_sources(led_ON_OFF INTERFACE
    matrix.cpp
    worker.cpp
)

target_link_libraries(led_ON_OFF INTERFACE
    pico_multicore
    hardware_dma
    hardware_pio
    hardware_timer
)

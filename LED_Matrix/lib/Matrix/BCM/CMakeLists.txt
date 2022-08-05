# Since we use preprocessor we have to use interface library
#   Optimization likely destroys any point in making this an actual lib

add_library(led_BCM INTERFACE)

target_sources(led_BCM INTERFACE
    matrix.cpp
    worker.cpp
)

target_link_libraries(led_BCM INTERFACE
    pico_multicore
    hardware_dma
    hardware_pio
    hardware_timer
    led_multiplex
)

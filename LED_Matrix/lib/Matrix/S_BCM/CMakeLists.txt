# Since we use preprocessor we have to use interface library
#   Optimization likely destroys any point in making this an actual lib

add_library(led_S_BCM INTERFACE)

target_sources(led_S_BCM INTERFACE
    matrix.cpp
    worker.cpp
    calculator.cpp
)

target_link_libraries(led_S_BCM INTERFACE
    pico_multicore
    hardware_dma
    hardware_pio
    hardware_timer
)

# Since we use preprocessor we have to use interface library
#   Optimization likely destroys any point in making this an actual lib

add_library(led_HUB75_BCM INTERFACE)

target_sources(led_HUB75_BCM INTERFACE
    matrix.cpp
    worker.cpp
    calculator.cpp
    loafer.cpp
)

target_link_libraries(led_HUB75_BCM INTERFACE
    pico_multicore
    hardware_dma
    hardware_pio
    hardware_timer
)

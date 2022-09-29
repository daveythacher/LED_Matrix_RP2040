# Since we use preprocessor we have to use interface library
#   Optimization likely destroys any point in making this an actual lib

add_library(led_PWM INTERFACE)

target_sources(led_PWM INTERFACE
    matrix.cpp
    worker.cpp
)

target_link_libraries(led_PWM INTERFACE
    pico_multicore
    hardware_dma
    hardware_pio
    hardware_timer
    led_multiplex
)

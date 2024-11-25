# Since we use preprocessor we have to use interface library
#   Optimization likely destroys any point in making this an actual lib

add_library(led_HUB75_PWM INTERFACE)

target_sources(led_HUB75_PWM INTERFACE
    matrix.cpp
    worker.cpp
    Buffer.cpp
    calculator.cpp
)

target_link_libraries(led_HUB75_PWM INTERFACE
    pico_multicore
    hardware_dma
    hardware_pio
    hardware_timer
    led_SIMD
    led_multiplex_${DEFINE_MULTIPLEX_FOLDER}_${DEFINE_MULTIPLEX_ALGORITHM}
)

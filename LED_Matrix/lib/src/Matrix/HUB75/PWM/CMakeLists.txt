add_subdirectory(PWM_Worker)

# Since we use preprocessor we have to use interface library
#   Optimization likely destroys any point in making this an actual lib

add_library(led_HUB75_PWM INTERFACE)

target_sources(led_HUB75_PWM INTERFACE
    PWM_Matrix.cpp
    PWM_Packet.cpp
    PWM_Calculator.cpp
    PWM_Programs.cpp
    PWM_Multiplex.cpp
)

target_link_libraries(led_HUB75_PWM INTERFACE
    pico_multicore
    hardware_dma
    hardware_pio
    led_SIMD
    led_PIO
    led_HUB75_PWM_Worker
    led_multiplex_${DEFINE_MULTIPLEX_FOLDER}_${DEFINE_MULTIPLEX_ALGORITHM}
)

# Since we use preprocessor we have to use interface library
#   Optimization likely destroys any point in making this an actual lib

add_library(led_HUB75_PWM_Worker_LUT INTERFACE)

target_sources(led_HUB75_PWM_Worker_LUT INTERFACE
    PWM_Worker_LUT.cpp
)

target_link_libraries(led_HUB75_PWM_Worker_LUT INTERFACE
    led_SIMD
)

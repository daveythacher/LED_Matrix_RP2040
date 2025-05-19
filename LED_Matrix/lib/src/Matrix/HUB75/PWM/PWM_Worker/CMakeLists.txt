add_subdirectory(LUT)

add_library(led_HUB75_PWM_Worker INTERFACE)

target_link_libraries(led_HUB75_PWM_Worker INTERFACE
    led_HUB75_PWM_Worker_LUT
    # TODO: Make this configurable
)
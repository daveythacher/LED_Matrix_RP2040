add_library(led_PIO_SM INTERFACE)

target_sources(led_PIO_SM INTERFACE
    SM.cpp
)

target_link_libraries(led_PIO_SM INTERFACE
    hardware_gpio
    hardware_pio
    led_GPIO
)

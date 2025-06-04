add_library(led_gpio INTERFACE)

target_sources(led_gpio INTERFACE
    GPIO.cpp
)

target_link_libraries(led_gpio INTERFACE
    hardware_claim
    hardware_gpio
)
add_library(led_GPIO INTERFACE)

target_sources(led_GPIO INTERFACE
    GPIO.cpp
)

target_link_libraries(led_GPIO INTERFACE
    hardware_claim
    hardware_gpio
)
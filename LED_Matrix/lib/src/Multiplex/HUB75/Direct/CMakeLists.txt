# This could possibly be a static library
add_library(led_multiplex_HUB75_Direct INTERFACE)

target_sources(led_multiplex_HUB75_Direct INTERFACE
    Direct.cpp
)

target_link_libraries(led_multiplex_HUB75_Direct INTERFACE 
    hardware_gpio
    hardware_irq
)

add_library(led_multiplex_Direct INTERFACE)

target_sources(led_multiplex_Direct INTERFACE
    Direct.cpp
)

target_link_libraries(led_multiplex_Direct INTERFACE 
    hardware_gpio
    hardware_irq
)

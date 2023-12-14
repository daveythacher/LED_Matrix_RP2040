# This could possibly be a static library
add_library(led_multiplex_Shifter_ACB INTERFACE)

target_sources(led_multiplex_Shifter_ACB INTERFACE
    Shifter_ACB.cpp
)

target_link_libraries(led_multiplex_Shifter_ACB INTERFACE 
    hardware_gpio
    hardware_irq
)

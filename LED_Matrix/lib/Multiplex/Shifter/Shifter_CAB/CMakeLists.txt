# This could possibly be a static library
add_library(led_multiplex_Shifter_CAB INTERFACE)

target_sources(led_multiplex_Shifter_CAB INTERFACE
    Shifter_CAB.cpp
)

target_link_libraries(led_multiplex_Shifter_CAB INTERFACE 
    hardware_gpio
    hardware_irq
)

# This could possibly be a static library
add_library(led_multiplex_Shifter_BAC INTERFACE)

target_sources(led_multiplex_Shifter_BAC INTERFACE
    Shifter_BAC.cpp
)

target_link_libraries(led_multiplex_Shifter_BAC INTERFACE 
    hardware_gpio
    hardware_irq
)

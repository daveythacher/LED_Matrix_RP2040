# This could possibly be a static library
add_library(led_multiplex_Shifter_CBA INTERFACE)

target_sources(led_multiplex_Shifter_CBA INTERFACE
    Shifter_CBA.cpp
)

target_link_libraries(led_multiplex_Shifter_CBA INTERFACE 
    hardware_gpio
    hardware_irq
)

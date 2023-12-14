# This could possibly be a static library
add_library(led_multiplex_Shifter_ABC INTERFACE)

target_sources(led_multiplex_Shifter_ABC INTERFACE
    Shifter_ABC.cpp
)

target_link_libraries(led_multiplex_Shifter_ABC INTERFACE 
    hardware_gpio
    hardware_irq
)

# This could possibly be a static library
add_library(led_multiplex_Shifter_ABC_Direct_DE INTERFACE)

target_sources(led_multiplex_Shifter_ABC_Direct_DE INTERFACE
    Shifter_ABC_Direct_DE.cpp
)

target_link_libraries(led_multiplex_Shifter_ABC_Direct_DE INTERFACE 
    hardware_gpio
    hardware_irq
)

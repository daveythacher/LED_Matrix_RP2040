# This could possibly be a static library
add_library(led_multiplex_Shifter INTERFACE)

target_sources(led_multiplex_Shifter INTERFACE
    Shifter.cpp
)

target_link_libraries(led_multiplex_Shifter INTERFACE 
    hardware_gpio
    hardware_pio
    hardware_irq
)

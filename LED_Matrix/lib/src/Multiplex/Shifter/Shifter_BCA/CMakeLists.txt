# This could possibly be a static library
add_library(led_multiplex_Shifter_BCA INTERFACE)

target_sources(led_multiplex_Shifter_BCA INTERFACE
    Shifter_BCA.cpp
)

target_link_libraries(led_multiplex_Shifter_BCA INTERFACE 
    hardware_gpio
    hardware_irq
)

add_library(led_multiplex_BUS8_Decoder INTERFACE)

target_sources(led_multiplex_BUS8_Decoder INTERFACE
    Decoder.cpp
    Programs.cpp
)

target_link_libraries(led_multiplex_BUS8_Decoder INTERFACE 
    hardware_gpio
    hardware_pio
    hardware_dma
    led_pio
    led_system
)

# This could possibly be a static library
add_library(led_multiplex_HUB75_Decoder INTERFACE)

target_sources(led_multiplex_HUB75_Decoder INTERFACE
    Decoder.cpp
)

target_link_libraries(led_multiplex_HUB75_Decoder INTERFACE 
    hardware_gpio
)

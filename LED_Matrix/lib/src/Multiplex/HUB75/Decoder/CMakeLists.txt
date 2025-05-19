add_library(led_multiplex_HUB75_Decoder INTERFACE)

target_sources(led_multiplex_HUB75_Decoder INTERFACE
    Decoder.cpp
    Programs.cpp
)

target_link_libraries(led_multiplex_HUB75_Decoder INTERFACE 
    hardware_gpio
    hardware_pio
    hardware_dma
    led_multiplex_mapper_${DEFINE_MULTIPLEX_FOLDER}_${DEFINE_MULTIPLEX_MAPPER}
)

# Since we use preprocessor we have to use interface library
#   Optimization likely destroys any point in making this an actual lib

add_library(led_BUS8_SPWM INTERFACE)

target_sources(led_BUS8_SPWM INTERFACE
    Matrix.cpp
    Packet.cpp
    Programs.cpp
    Multiplex.cpp
)

target_link_libraries(led_BUS8_SPWM INTERFACE
    pico_multicore
    hardware_dma
    hardware_gpio
    hardware_pio
    led_pio
    led_multiplex
    led_system
)

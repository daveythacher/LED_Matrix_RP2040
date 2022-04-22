add_library(led_SPWM INTERFACE)
target_sources(led_SPWM INTERFACE
    matrix.cpp
    worker.cpp
)
target_link_libraries(led_SPWM INTERFACE
    pico_stdlib
    hardware_dma
    hardware_pio
    hardware_timer
    led_multiplex
)

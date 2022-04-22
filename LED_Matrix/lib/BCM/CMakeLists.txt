add_library(led_BCM INTERFACE)
target_sources(led_BCM INTERFACE
    matrix.cpp
    worker.cpp
)
target_link_libraries(led_BCM INTERFACE
    pico_stdlib
    hardware_dma
    hardware_pio
    hardware_timer
    led_multiplex
)

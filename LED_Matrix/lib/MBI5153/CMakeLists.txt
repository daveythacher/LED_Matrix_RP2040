add_library(led_MBI5153 INTERFACE)
target_sources(led_MBI5153 INTERFACE
    matrix.cpp
    worker.cpp
)
target_link_libraries(led_MBI5153 INTERFACE
    pico_stdlib
    hardware_dma
    hardware_pio
    hardware_timer
    led_multiplex
)

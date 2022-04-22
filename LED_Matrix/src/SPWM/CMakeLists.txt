target_sources(led_SPWM PRIVATE
    matrix.cpp
    serial.cpp
    worker.cpp
)
target_link_libraries(led_SPWM 
    pico_stdlib
    hardware_dma
    hardware_pio
    hardware_timer
    pico_multicore
)

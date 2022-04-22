target_sources(led_BCM PRIVATE
    matrix.cpp
    serial.cpp
    worker.cpp
)
target_link_libraries(led_BCM
    pico_stdlib
    hardware_dma
	hardware_pio
	hardware_timer
	pico_multicore
)


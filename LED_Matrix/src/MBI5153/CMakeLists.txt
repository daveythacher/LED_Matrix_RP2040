target_sources(led_MBI5153 PRIVATE
    matrix.cpp
    serial.cpp
    worker.cpp
)
target_link_libraries(led_MBI5153 
    pico_stdlib
    hardware_dma
	hardware_pio
	hardware_timer
	pico_multicore
)

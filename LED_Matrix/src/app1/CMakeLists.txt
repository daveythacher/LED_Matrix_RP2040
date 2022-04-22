target_sources(led_app1 PRIVATE
    serial.cpp
)
target_link_libraries(led_app1 
    pico_stdlib
    pico_multicore
    led_BCM
)

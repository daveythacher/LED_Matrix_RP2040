target_sources(led_app2 PRIVATE
    serial.cpp
)
target_link_libraries(led_app2 
    pico_stdlib
    pico_multicore
    led_SPWM
)

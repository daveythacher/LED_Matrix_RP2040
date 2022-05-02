target_sources(led_${TARGET} PRIVATE
    serial.cpp
    isr.cpp)
target_link_libraries(led_${TARGET} 
    pico_stdlib
    pico_multicore
    led_SPWM
    FreeRTOS
)

pico_set_binary_type(led_${TARGET} copy_to_ram)

# enable usb output, disable uart output
pico_enable_stdio_usb(led_${TARGET} 1)
pico_enable_stdio_uart(led_${TARGET} 0)

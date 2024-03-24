add_library(serial_uart INTERFACE)

target_sources(serial_uart INTERFACE
    serial_uart.cpp
    serial.cpp
)

# Use caution here!
target_link_libraries(serial_uart INTERFACE 
    hardware_gpio
    hardware_irq
    hardware_uart
    hardware_dma        # Cannot use full API!
    hardware_timer
    hardware_resets
    hardware_clocks
)

add_library(serial_test INTERFACE)

target_sources(serial_test INTERFACE
    serial.cpp
)

# Use caution here!
target_link_libraries(serial_test INTERFACE 
    hardware_gpio
    hardware_uart
    hardware_dma        # Cannot use full API!
    hardware_timer
    hardware_resets
    hardware_clocks
)

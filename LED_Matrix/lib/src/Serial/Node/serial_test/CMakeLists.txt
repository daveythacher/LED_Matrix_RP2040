add_library(serial_node_test INTERFACE)

target_sources(serial_node_test INTERFACE
    serial.cpp
    isr.cpp
)

# Use caution here!
target_link_libraries(serial_node_test INTERFACE 
    hardware_gpio
    hardware_irq
    hardware_uart
    hardware_dma        # Cannot use full API!
    hardware_timer
    hardware_resets
    hardware_clocks
)

add_library(serial_node_uart INTERFACE)

target_sources(serial_node_uart INTERFACE
    Node.cpp
)

# Use caution here!
target_link_libraries(serial_node_uart INTERFACE 
    hardware_gpio
    hardware_uart
    led_GPIO
)

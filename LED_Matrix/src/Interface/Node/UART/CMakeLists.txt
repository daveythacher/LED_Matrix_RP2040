add_library(led_interface_node_uart INTERFACE)

target_sources(led_interface_node_uart INTERFACE
    UART.cpp
)

# Use caution here!
target_link_libraries(led_interface_node_uart INTERFACE 
    hardware_gpio
    hardware_uart
    led_GPIO
)

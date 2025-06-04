add_library(led_interface_protocol_serial_command_control_swap INTERFACE)

target_sources(led_interface_protocol_serial_command_control_swap INTERFACE
    Swap.cpp
)

# Use caution here!
target_link_libraries(led_interface_protocol_serial_command_control_swap INTERFACE
    led_system
)
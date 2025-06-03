add_library(led_interface_protocol_serial_command_control_power INTERFACE)

target_sources(led_interface_protocol_serial_command_control_power INTERFACE
    Power.cpp
)

# Use caution here!
target_link_libraries(led_interface_protocol_serial_command_control_power INTERFACE
    led_system
)
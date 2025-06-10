add_library(led_interface_protocol_serial_command_control_reboot INTERFACE)

target_sources(led_interface_protocol_serial_command_control_reboot INTERFACE
    Reboot.cpp
)

# Use caution here!
target_link_libraries(led_interface_protocol_serial_command_control_reboot INTERFACE
    led_system
)
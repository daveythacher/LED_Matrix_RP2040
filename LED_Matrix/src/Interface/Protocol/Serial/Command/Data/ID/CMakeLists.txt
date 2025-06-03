add_library(led_interface_protocol_serial_command_data_id INTERFACE)

target_sources(led_interface_protocol_serial_command_data_id INTERFACE
    ID.cpp
)

# Use caution here!
target_link_libraries(led_interface_protocol_serial_command_data_id INTERFACE
    led_system
)
add_library(led_interface_protocol_serial_command_data_data INTERFACE)

target_sources(led_interface_protocol_serial_command_data_data INTERFACE
    Data.cpp
)

# Use caution here!
target_link_libraries(led_interface_protocol_serial_command_data_data INTERFACE
    led_system
)
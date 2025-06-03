add_library(interface_protocol_serial_command_data_data INTERFACE)

target_sources(interface_protocol_serial_command_data_data INTERFACE
    Data.cpp
)

# Use caution here!
target_link_libraries(interface_protocol_serial_command_data_data INTERFACE
    led_system
)
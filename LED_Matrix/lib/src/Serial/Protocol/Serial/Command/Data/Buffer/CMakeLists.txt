add_library(serial_protocol_serial_command_data_buffer INTERFACE)

target_sources(serial_protocol_serial_command_data_buffer INTERFACE
    Buffer.cpp
)

# Use caution here!
target_link_libraries(serial_protocol_serial_command_data_buffer INTERFACE
    led_system
)
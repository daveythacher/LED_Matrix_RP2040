add_library(serial_protocol_serial_command_data_id INTERFACE)

target_sources(serial_protocol_serial_command_data_id INTERFACE
    ID.cpp
)

# Use caution here!
target_link_libraries(serial_protocol_serial_command_data_id INTERFACE
    led_system
)
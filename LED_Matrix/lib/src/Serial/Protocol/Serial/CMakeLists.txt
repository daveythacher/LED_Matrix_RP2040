add_subdirectory(Command)

add_library(serial_protocol_serial INTERFACE)

target_sources(serial_protocol_serial INTERFACE
    control_node.cpp
    internal.cpp
    Status_Message.cpp
    serial.cpp
    filter.cpp
)

# Use caution here!
target_link_libraries(serial_protocol_serial INTERFACE
    hardware_uart
    serial_protocol_serial_command
    serial_protocol_serial_command_data_data
    serial_protocol_serial_command_data_buffer
    serial_protocol_serial_command_data_id
    serial_protocol_serial_command_data_raw
    serial_protocol_serial_command_query_test
    led_SIMD
    led_TCAM 
    led_system
)

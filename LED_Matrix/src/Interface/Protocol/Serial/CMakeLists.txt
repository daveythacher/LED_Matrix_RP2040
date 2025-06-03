add_subdirectory(Command)

add_library(led_interface_protocol_serial INTERFACE)

target_sources(led_interface_protocol_serial INTERFACE
    #internal.cpp
    #Status_Message.cpp
    Serial.cpp
    Header.cpp
)

# Use caution here!
target_link_libraries(led_interface_protocol_serial INTERFACE
    #serial_protocol_serial_command
    #serial_protocol_serial_command_data_data
    #serial_protocol_serial_command_data_buffer
    #serial_protocol_serial_command_data_id
    #serial_protocol_serial_command_data_raw
    #serial_protocol_serial_command_query_test
    #led_system
    #led_matrix
    led_interface_node_${DEFINE_SERIAL_NODE}
)

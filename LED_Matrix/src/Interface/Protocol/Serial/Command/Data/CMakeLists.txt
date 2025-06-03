add_subdirectory(Data)
add_subdirectory(ID)

add_library(led_interface_protocol_serial_command_data INTERFACE)

# Use caution here!
target_link_libraries(led_interface_protocol_serial_command_data INTERFACE
    led_interface_protocol_serial_command_data_data
    led_interface_protocol_serial_command_data_id
)
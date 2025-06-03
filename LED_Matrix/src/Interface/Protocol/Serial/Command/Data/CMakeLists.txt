add_subdirectory(Data)
add_subdirectory(ID)

add_library(interface_protocol_serial_command_data INTERFACE)

# Use caution here!
target_link_libraries(interface_protocol_serial_command_data INTERFACE
    interface_protocol_serial_command_data_data
    interface_protocol_serial_command_data_id
)
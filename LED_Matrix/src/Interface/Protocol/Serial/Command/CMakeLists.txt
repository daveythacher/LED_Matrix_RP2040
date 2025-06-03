add_subdirectory(Data)
add_subdirectory(Query)

add_library(interface_protocol_serial_command INTERFACE)

# Use caution here!
target_link_libraries(interface_protocol_serial_command INTERFACE
    interface_protocol_serial_command_data
    interface_protocol_serial_command_query
)
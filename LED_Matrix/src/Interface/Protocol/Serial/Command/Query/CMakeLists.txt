add_subdirectory(Test)

add_library(interface_protocol_serial_command_query INTERFACE)

# Use caution here!
target_link_libraries(interface_protocol_serial_command_query INTERFACE
    interface_protocol_serial_command_query_test
)
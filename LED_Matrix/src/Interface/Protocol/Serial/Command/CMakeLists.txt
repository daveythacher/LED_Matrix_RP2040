add_subdirectory(Data)
add_subdirectory(Query)

add_library(led_interface_protocol_serial_command INTERFACE)

target_sources(led_interface_protocol_serial_command INTERFACE
    Command.cpp
)

# Use caution here!
target_link_libraries(led_interface_protocol_serial_command INTERFACE
    led_interface_protocol_serial_command_data
    led_interface_protocol_serial_command_query
)
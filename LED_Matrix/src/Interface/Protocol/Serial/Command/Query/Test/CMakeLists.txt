add_library(led_interface_protocol_serial_command_query_test INTERFACE)

target_sources(led_interface_protocol_serial_command_query_test INTERFACE
    Test.cpp
)

# Use caution here!
target_link_libraries(led_interface_protocol_serial_command_query_test INTERFACE
    led_system
)
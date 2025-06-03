add_library(interface_protocol_serial_command_query_test INTERFACE)

target_sources(interface_protocol_serial_command_query_test INTERFACE
    Test.cpp
)

# Use caution here!
target_link_libraries(interface_protocol_serial_command_query_test INTERFACE
    led_system
)
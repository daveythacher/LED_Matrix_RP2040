add_library(serial_protocol_serial_command_query_test INTERFACE)

target_sources(serial_protocol_serial_command_query_test INTERFACE
    Test.cpp
)

# Use caution here!
target_link_libraries(serial_protocol_serial_command_query_test INTERFACE
    led_system
)
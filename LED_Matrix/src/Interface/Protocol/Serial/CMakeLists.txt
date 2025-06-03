add_subdirectory(Command)

add_library(led_interface_protocol_serial INTERFACE)

target_sources(led_interface_protocol_serial INTERFACE
    Serial.cpp
    Header.cpp
)

# Use caution here!
target_link_libraries(led_interface_protocol_serial INTERFACE
    led_interface_protocol_serial_command
    led_interface_node_${DEFINE_SERIAL_NODE}
    led_system
)

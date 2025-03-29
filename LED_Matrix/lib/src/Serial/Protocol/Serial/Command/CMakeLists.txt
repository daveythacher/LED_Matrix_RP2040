add_subdirectory(Data)
add_subdirectory(Query)

add_library(serial_protocol_serial_command INTERFACE)

target_sources(serial_protocol_serial_command INTERFACE
    Command.cpp
)

# Use caution here!
target_link_libraries(serial_protocol_serial_command INTERFACE
    led_SIMD
    led_system
)
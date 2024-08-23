add_library(serial_protocol_serial INTERFACE)

target_sources(serial_protocol_serial INTERFACE
    control_node.cpp
    internal.cpp
    Status_Message.cpp
    serial.cpp
    Command/Command.cpp
    Command/filter.cpp
)

# Use caution here!
target_link_libraries(serial_protocol_serial INTERFACE
    hardware_uart
)

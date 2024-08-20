add_library(serial_protocol_serial INTERFACE)

target_sources(serial_protocol_serial INTERFACE
    control_node.cpp
    data_node.cpp
    internal.cpp
    Status_Message.cpp
)

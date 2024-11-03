add_subdirectory(Data)
add_subdirectory(Query)

add_library(serial_protocol_serial_command INTERFACE)

target_sources(serial_protocol_serial_command INTERFACE
    Command.cpp
)
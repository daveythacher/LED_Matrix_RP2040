add_subdirectory(Serial)

add_library(led_interface_protocol INTERFACE)

target_sources(led_interface_protocol INTERFACE
    Protocol.cpp
)

target_link_libraries(led_interface_protocol INTERFACE
    led_interface_protocol_serial
)
add_subdirectory(Serial)

add_library(led_interface_protocol INTERFACE)

target_link_libraries(led_interface_protocol INTERFACE
    led_interface_protocol_serial
)
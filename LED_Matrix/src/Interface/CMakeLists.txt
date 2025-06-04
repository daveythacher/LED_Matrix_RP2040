add_subdirectory(Protocol)
add_subdirectory(Node)

add_library(led_interface INTERFACE)

target_link_libraries(led_interface INTERFACE
    led_interface_protocol
    led_interface_node
)
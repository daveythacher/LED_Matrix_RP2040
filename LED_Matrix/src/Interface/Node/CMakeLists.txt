add_subdirectory(UART)

add_library(led_interface_node INTERFACE)

target_sources(led_interface_node INTERFACE
    Node.cpp
)

target_link_libraries(led_interface_node INTERFACE
    led_interface_node_uart
)
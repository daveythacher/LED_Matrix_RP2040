add_subdirectory(UART)

add_library(led_interface_node INTERFACE)

target_link_libraries(led_interface_node INTERFACE
    led_interface_node_uart
)
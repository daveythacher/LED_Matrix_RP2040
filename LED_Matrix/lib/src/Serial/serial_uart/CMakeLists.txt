add_library(serial_uart INTERFACE)

target_sources(serial_uart INTERFACE
    serial_uart.cpp
    serial.cpp
    isr.cpp
    control_node.cpp
    data_node.cpp
    internal.cpp
    tcam.cpp
    Status_Message.cpp
)

# Use caution here!
target_link_libraries(serial_uart INTERFACE 
    hardware_gpio
    hardware_uart
)

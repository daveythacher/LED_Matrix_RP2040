add_library(serial_node_test INTERFACE)

target_sources(serial_node_test INTERFACE
    control_node.cpp
    data_node.cpp
    isr.cpp
)

# Use caution here!
target_link_libraries(serial_node_test INTERFACE 
    hardware_irq
)

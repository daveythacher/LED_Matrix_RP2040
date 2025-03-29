add_library(serial_node_test INTERFACE)

target_sources(serial_node_test INTERFACE
    control_node.cpp
    data_node.cpp
)

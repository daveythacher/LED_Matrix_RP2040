# This could possibly be a static library
add_library(led_Queue INTERFACE)

target_sources(led_Queue INTERFACE
    Queue.cpp
)

target_link_libraries(led_Queue INTERFACE 
    FreeRTOS-Kernel
)

# This could possibly be a static library
add_library(led_Mutex INTERFACE)

target_sources(led_Mutex INTERFACE
    Mutex.cpp
)

target_link_libraries(led_Mutex INTERFACE 
    FreeRTOS-Kernel
)

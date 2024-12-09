# This could possibly be a static library
add_library(led_Thread INTERFACE)

target_sources(led_Thread INTERFACE
    Thread.cpp
)

target_link_libraries(led_Thread INTERFACE 
    FreeRTOS-Kernel
)

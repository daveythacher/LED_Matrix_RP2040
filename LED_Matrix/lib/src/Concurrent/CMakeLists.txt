add_subdirectory(Thread)
add_subdirectory(Queue)

# This could possibly be a static library
add_library(led_Concurrent INTERFACE)

target_link_libraries(led_Concurrent INTERFACE 
    led_Thread
    led_Queue
)

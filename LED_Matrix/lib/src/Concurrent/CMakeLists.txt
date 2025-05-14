add_subdirectory(Thread)
add_subdirectory(Queue)
add_subdirectory(Mutex)

# This could possibly be a static library
add_library(led_Concurrent INTERFACE)

target_link_libraries(led_Concurrent INTERFACE 
    led_Thread
    led_Queue
    led_Mutex
)

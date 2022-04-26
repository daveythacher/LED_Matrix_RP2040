target_sources(led_app3 PRIVATE
    serial.cpp
    isr.cpp
    ../../FreeRTOS-Kernel/list.c
    ../../FreeRTOS-Kernel/queue.c
    ../../FreeRTOS-Kernel/tasks.c
    ../../FreeRTOS-Kernel/portable/GCC/ARM_CM0/port.c 
    ../../FreeRTOS_helpers.c)
target_include_directories(led_app3 PUBLIC 
    ../../FreeRTOS-Kernel/include 
    ../../FreeRTOS-Kernel/portable/GCC/ARM_CM0)
target_link_libraries(led_app3 
    pico_stdlib
    pico_multicore
    led_MBI5153
)

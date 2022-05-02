add_library(FreeRTOS INTERFACE)
target_sources(FreeRTOS INTERFACE
    FreeRTOS-Kernel/list.c
    FreeRTOS-Kernel/queue.c
    FreeRTOS-Kernel/tasks.c
    FreeRTOS-Kernel/portable/GCC/ARM_CM0/port.c 
    FreeRTOS_helpers.c
)
target_include_directories(FreeRTOS INTERFACE
    ../../include/FreeRTOS
    FreeRTOS-Kernel/include 
    FreeRTOS-Kernel/portable/GCC/ARM_CM0)
target_link_libraries(FreeRTOS INTERFACE pico_stdlib)

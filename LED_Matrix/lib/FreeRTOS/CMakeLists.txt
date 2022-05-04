add_library(FreeRTOS)

target_sources(FreeRTOS PRIVATE
    FreeRTOS-Kernel/list.c
    FreeRTOS-Kernel/queue.c
    FreeRTOS-Kernel/tasks.c
    FreeRTOS-Kernel/portable/GCC/ARM_CM0/port.c 
    FreeRTOS_helpers.c
)

target_include_directories(FreeRTOS PRIVATE
    ../../include/FreeRTOS
    FreeRTOS-Kernel/include 
    FreeRTOS-Kernel/portable/GCC/ARM_CM0
)

target_compile_options(FreeRTOS PRIVATE 
    $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
    -fno-exceptions 
    -fno-check-new 
    $<$<COMPILE_LANGUAGE:CXX>:-fno-enforce-eh-specs>
    -g 
    -ffunction-sections 
    -fdata-sections 
    -O3
    -funroll-loops 
    -Werror 
    -Wall
)

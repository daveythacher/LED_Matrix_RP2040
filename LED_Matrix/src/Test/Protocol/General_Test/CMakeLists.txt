add_executable(led_${DEFINE_APP}_Protocol_General_Test 
    ./main.cpp
)

target_include_directories(led_${DEFINE_APP}_Protocol_General_Test PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}/../../../../include
    ../../../../include
)

target_compile_options(led_${DEFINE_APP}_Protocol_General_Test PRIVATE 
    $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
    -fno-exceptions 
    -fno-check-new 
    $<$<COMPILE_LANGUAGE:CXX>:-fno-enforce-eh-specs>
    -ffunction-sections 
    -fdata-sections 
    -fstack-usage
    -funroll-loops 
    -Werror 
    -Wall
)

target_compile_definitions(led_${DEFINE_APP}_Protocol_General_Test PRIVATE 
    PICO_HEAP_SIZE=196608
    PICO_STACK_SIZE=4096
    PICO_CORE1_STACK_SIZE=4096
    PICO_XOSC_STARTUP_DELAY_MULTIPLIER=64
)

pico_set_binary_type(led_${DEFINE_APP}_Protocol_General_Test copy_to_ram)

# enable usb output, disable uart output
pico_enable_stdio_usb(led_${DEFINE_APP}_Protocol_General_Test 1)
pico_enable_stdio_uart(led_${DEFINE_APP}_Protocol_General_Test 0)
    
target_link_libraries(led_${DEFINE_APP}_Protocol_General_Test 
    pico_stdio
    pico_runtime
    pico_multicore
    led_matrix
    led_system
    led_interface
)

target_link_options(led_${DEFINE_APP}_Protocol_General_Test 
    PRIVATE "LINKER:--print-memory-usage"
)

# create map/bin/hex file etc.
pico_add_extra_outputs(led_${DEFINE_APP}_Protocol_General_Test)
pico_add_dis_output2(led_${DEFINE_APP}_Protocol_General_Test)
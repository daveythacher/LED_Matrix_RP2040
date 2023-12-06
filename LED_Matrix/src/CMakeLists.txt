add_executable(led 
    ./main.cpp
)

target_include_directories(led PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}/../include
    ../include
)

target_compile_options(led PRIVATE 
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

target_compile_definitions(led PRIVATE 
    PICO_HEAP_SIZE=2048
    PICO_STACK_SIZE=4096
    PICO_CORE1_STACK_SIZE=4096
    PICO_XOSC_STARTUP_DELAY_MULTIPLIER=64
)

pico_set_binary_type(led copy_to_ram)

# enable usb output, disable uart output
pico_enable_stdio_usb(led 1)
pico_enable_stdio_uart(led 0)
    
target_link_libraries(led 
    pico_stdio_usb
    pico_runtime
    pico_multicore
    led_${DEFINE_ALGORITHM}
    led_multiplex_${DEFINE_MULTIPLEX_NAME}
    serial_${DEFINE_APP}
)

target_link_options(led 
    PRIVATE "LINKER:--print-memory-usage"
)

# create map/bin/hex file etc.
pico_add_extra_outputs(led)
pico_add_dis_output2(led)

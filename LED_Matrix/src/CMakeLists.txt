add_executable(led_${DEFINE_APP}_front 
    ./front.cpp
)

target_include_directories(led_${DEFINE_APP}_front PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}/../include
    ${CMAKE_CURRENT_BINARY_DIR}/../lib/include
    ../include
    ../lib/include
)

target_compile_options(led_${DEFINE_APP}_front PRIVATE 
    $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
    -fno-exceptions 
    -fno-check-new 
    $<$<COMPILE_LANGUAGE:CXX>:-fno-enforce-eh-specs>
    -g 
    -ffunction-sections 
    -fdata-sections 
    -fstack-usage
    -O3
    -funroll-loops 
    -Werror 
    -Wall
)

target_compile_definitions(led_${DEFINE_APP}_front PRIVATE 
    PICO_HEAP_SIZE=2048
    PICO_STACK_SIZE=4096
    PICO_CORE1_STACK_SIZE=4096
    PICO_XOSC_STARTUP_DELAY_MULTIPLIER=64
    DEFINE_IS_LOAFER="false"
)

pico_set_binary_type(led_${DEFINE_APP}_front copy_to_ram)

# enable usb output, disable uart output
pico_enable_stdio_usb(led_${DEFINE_APP}_front 1)
pico_enable_stdio_uart(led_${DEFINE_APP}_front 0)
    
target_link_libraries(led_${DEFINE_APP}_front 
    pico_stdio_usb
    pico_runtime
    pico_multicore
    led_${DEFINE_MATRIX_FOLDER}_${DEFINE_MATRIX_ALGORITHM}
    led_multiplex_${DEFINE_MULTIPLEX_ALGORITHM}
    serial_${DEFINE_SERIAL_ALGORITHM}
)

target_link_options(led_${DEFINE_APP}_front 
    PRIVATE "LINKER:--print-memory-usage"
)

# create map/bin/hex file etc.
pico_add_extra_outputs(led_${DEFINE_APP}_front)
pico_add_dis_output2(led_${DEFINE_APP}_front)

add_executable(led_${DEFINE_APP}_back 
    ./back.cpp
)

target_include_directories(led_${DEFINE_APP}_back PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}/../include
    ${CMAKE_CURRENT_BINARY_DIR}/../lib/include
    ../include
    ../lib/include
)

target_compile_options(led_${DEFINE_APP}_back PRIVATE 
    $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
    -fno-exceptions 
    -fno-check-new 
    $<$<COMPILE_LANGUAGE:CXX>:-fno-enforce-eh-specs>
    -g 
    -ffunction-sections 
    -fdata-sections 
    -fstack-usage
    -O3
    -funroll-loops 
    -Werror 
    -Wall
)

target_compile_definitions(led_${DEFINE_APP}_back PRIVATE 
    PICO_HEAP_SIZE=2048
    PICO_STACK_SIZE=4096
    PICO_CORE1_STACK_SIZE=4096
    PICO_XOSC_STARTUP_DELAY_MULTIPLIER=64
    DEFINE_IS_LOAFER="true"
)

pico_set_binary_type(led_${DEFINE_APP}_back copy_to_ram)

# enable usb output, disable uart output
pico_enable_stdio_usb(led_${DEFINE_APP}_back 1)
pico_enable_stdio_uart(led_${DEFINE_APP}_back 0)
    
target_link_libraries(led_${DEFINE_APP}_back 
    pico_stdio_usb
    pico_runtime
    pico_multicore
    led_${DEFINE_MATRIX_FOLDER}_${DEFINE_MATRIX_ALGORITHM}
    led_multiplex_${DEFINE_MULTIPLEX_ALGORITHM}
    serial_${DEFINE_SERIAL_ALGORITHM}
)

target_link_options(led_${DEFINE_APP}_back 
    PRIVATE "LINKER:--print-memory-usage"
)

# create map/bin/hex file etc.
pico_add_extra_outputs(led_${DEFINE_APP}_back)
pico_add_dis_output2(led_${DEFINE_APP}_back)

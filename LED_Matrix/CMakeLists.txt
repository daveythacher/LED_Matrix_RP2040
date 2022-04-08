cmake_minimum_required(VERSION 3.13)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

project(led C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the SDK
pico_sdk_init()

add_executable(led main.cpp worker.cpp matrix.cpp serial.cpp)
        
target_compile_definitions(led PUBLIC
	PICO_HEAP_SIZE=2048
	PICO_XOSC_STARTUP_DELAY_MULTIPLIER=64
)

target_compile_options(led PUBLIC
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

#pico_set_binary_type(led copy_to_ram)

# create map/bin/hex file etc.
pico_add_extra_outputs(led)

# enable usb output, disable uart output
pico_enable_stdio_usb(led 1)
pico_enable_stdio_uart(led 0)

# create disassembly with source
function(pico_add_dis_output2 TARGET)
    add_custom_command(TARGET ${TARGET} POST_BUILD
        COMMAND ${CMAKE_OBJDUMP} -S $<TARGET_FILE:${TARGET}> >$<IF:$<BOOL:$<TARGET_PROPERTY:${TARGET},OUTPUT_NAME>>,$<TARGET_PROPERTY:${TARGET},OUTPUT_NAME>,$<TARGET_PROPERTY:${TARGET},NAME>>.dis2)
endfunction()
pico_add_dis_output2(led)

# pull in dependencies
target_link_libraries(led 
	pico_stdlib
	hardware_dma
	hardware_pio
	pico_multicore)

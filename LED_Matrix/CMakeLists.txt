cmake_minimum_required(VERSION 3.13)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

project(led C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
    
# Initialize the SDK
pico_sdk_init()

# create disassembly with source
function(pico_add_dis_output2 TARGET)
    add_custom_command(TARGET ${TARGET} POST_BUILD
        COMMAND ${CMAKE_OBJDUMP} -S $<TARGET_FILE:${TARGET}> >$<IF:$<BOOL:$<TARGET_PROPERTY:${TARGET},OUTPUT_NAME>>,$<TARGET_PROPERTY:${TARGET},OUTPUT_NAME>,$<TARGET_PROPERTY:${TARGET},NAME>>.dis2)
    add_custom_command(TARGET ${TARGET} POST_BUILD
        COMMAND arm-none-eabi-size ${CMAKE_CURRENT_LIST_DIR}/../build/src/$<IF:$<BOOL:$<TARGET_PROPERTY:${TARGET},OUTPUT_NAME>>,$<TARGET_PROPERTY:${TARGET},OUTPUT_NAME>,$<TARGET_PROPERTY:${TARGET},NAME>>.elf
        VERBATIM
    )
endfunction()

set(DEFINE_APP "uart" CACHE STRING "Serial algorithm name")
set(DEFINE_SERIAL_CLOCK "17.0" CACHE STRING "Serial clock speed in MHz")
set(DEFINE_ALGORITHM "PWM" CACHE STRING "Matrix algorithm name")
set(DEFINE_SERIAL_RGB_TYPE "RGB24" CACHE STRING "RGB type name")
set(DEFINE_SERIAL_UART_BAUD "4000000" CACHE STRING "Serial algorithm baud rate in Baud")
set(DEFINE_MULTIPLEX "8" CACHE STRING "Panel scan")
set(DEFINE_MULTIPLEX_NAME "Decoder" CACHE STRING "Multiplex algorithm name")
set(DEFINE_GCLK "17.0" CACHE STRING "Grayscale clock speed in MHz")
set(DEFINE_COLUMNS "32" CACHE STRING "Shift chain length")
set(DEFINE_FPS "30" CACHE STRING "Frames per second")
set(DEFINE_RED_GAIN "1.0" CACHE STRING "Red current gain")
set(DEFINE_GREEN_GAIN "1.0" CACHE STRING "Green current gain")
set(DEFINE_BLUE_GAIN "1.0" CACHE STRING "Blue current gain")
set(DEFINE_BLANK_TIME "10" CACHE STRING "Blank time in microseconds")
set(DEFINE_MAX_REFRESH "3000" CACHE STRING "Refresh rate")
set(DEFINE_MAX_RGB_LED_STEPS "130" CACHE STRING "Min constrast of LED without multiplexing")

add_subdirectory(include)
add_subdirectory(lib)
add_subdirectory(src)

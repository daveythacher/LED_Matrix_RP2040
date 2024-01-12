cmake_minimum_required(VERSION 3.13)

# Pull in SDK (must be before project)
include(lib/pico-sdk/external/pico_sdk_import.cmake)

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

# This determines application name
set(DEFINE_APP "app" CACHE STRING "Binary name")

# These determine code modules (linker)
set(DEFINE_SERIAL_ALGORITHM "uart" CACHE STRING "Serial algorithm name")
set(DEFINE_MULTIPLEX_ALGORITHM "Decoder" CACHE STRING "Multiplex algorithm name")
set(DEFINE_MATRIX_ALGORITHM "PWM" CACHE STRING "Matrix algorithm name")

# These determine RAM usage
set(DEFINE_SERIAL_RGB_TYPE "RGB24" CACHE STRING "RGB type name")
set(DEFINE_MULTIPLEX_SCAN "8" CACHE STRING "Panel scan")
set(DEFINE_COLUMNS "32" CACHE STRING "Shift chain length")
set(DEFINE_MAX_RGB_LED_STEPS "130" CACHE STRING "Min constrast of LED without multiplexing")
set(DEFINE_S_PWM_SEG "4" CACHE STRING "Number of bits per S-PWM segment")

# These determine timing and state machine settings at compile time
set(DEFINE_MATRIX_DCLOCK "17.0" CACHE STRING "Matrix serial clock speed in MHz")
set(DEFINE_SERIAL_UART_BAUD "4000000" CACHE STRING "Serial algorithm baud rate in Baud")
set(DEFINE_BLANK_TIME "10" CACHE STRING "Blank time in microseconds")
set(DEFINE_IS_LOAFER "false" CACHE STRING "Select Serial Algorithm protocol mode")

# These verify the configuration settings at compile time
set(DEFINE_FPS "30" CACHE STRING "Frames per second")
set(DEFINE_MIN_REFRESH "3000" CACHE STRING "Refresh rate")
set(DEFINE_BYPASS_FANOUT "false" CACHE STRING "Disable verification of max Matrix algorithm Serial clock fanout speed limit")

add_subdirectory(include)
add_subdirectory(lib)
add_subdirectory(src)

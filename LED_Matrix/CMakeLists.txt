cmake_minimum_required(VERSION 3.13)

# Pull in SDK (must be before project)
include(lib/pico-sdk/external/pico_sdk_import.cmake)

project(led C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)    

# Hack for CMake
set(CMAKE_C_FLAGS_RELEASE "-O3 -g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -g")
set(CMAKE_ASM_FLAGS_RELEASE "-O3 -g")

# Initialize the SDK
pico_sdk_init()

# create disassembly with source
function(pico_add_dis_output2 TARGET)
    add_custom_command(TARGET ${TARGET} POST_BUILD
        COMMAND ${CMAKE_OBJDUMP} -S $<TARGET_FILE:${TARGET}> >$<IF:$<BOOL:$<TARGET_PROPERTY:${TARGET},OUTPUT_NAME>>,$<TARGET_PROPERTY:${TARGET},OUTPUT_NAME>,$<TARGET_PROPERTY:${TARGET},NAME>>.dis2)
    
    if (PICO_COMPILER STREQUAL "pico_arm_gcc")
        pico_find_compiler(PICO_COMPILER_SIZE ${PICO_GCC_TRIPLE}-size)
        add_custom_command(TARGET ${TARGET} POST_BUILD
            COMMAND ${PICO_COMPILER_SIZE} ${CMAKE_BINARY_DIR}/$<IF:$<BOOL:$<TARGET_PROPERTY:${TARGET},OUTPUT_NAME>>,$<TARGET_PROPERTY:${TARGET},OUTPUT_NAME>,$<TARGET_PROPERTY:${TARGET},NAME>>.elf
            VERBATIM
        )
    endif()
endfunction()

# This determines application name
set(DEFINE_APP "app" CACHE STRING "Binary name")

# These determine code modules (linker)
set(DEFINE_SERIAL_NODE "uart" CACHE STRING "Serial node algorithm name")
set(DEFINE_SERIAL_PROTOCOL "serial" CACHE STRING "Serial protocol algorithm name")
set(DEFINE_MULTIPLEX_ALGORITHM "Decoder" CACHE STRING "Multiplex algorithm name")
set(DEFINE_MULTIPLEX_FOLDER "BUS8" CACHE STRING "Multiplex interface family")
set(DEFINE_MATRIX_ALGORITHM "PWM" CACHE STRING "Matrix algorithm name")
set(DEFINE_MATRIX_FOLDER "BUS8" CACHE STRING "Matrix interface family")

add_subdirectory(src)

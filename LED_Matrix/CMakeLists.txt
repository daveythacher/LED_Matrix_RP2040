cmake_minimum_required(VERSION 3.13)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

project(led C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
    
# Initialize the SDK
pico_sdk_init()

set(DEFINE_MULTIPLEX 32 CACHE STRING "Multiplex Scan of panel")
set(DEFINE_MAX_RGB_LED_STEPS 8000 CACHE STRING "Min LED const forward current in nA divided by min light current in nA")
set(DEFINE_MAX_REFRESH 3840 CACHE STRING "Max refresh of panel")
set(DEFINE_FPS 30 CACHE STRING "Frames per second")
set(DEFINE_COLUMNS 128 CACHE STRING "Chain length")
set(DEFINE_MULTIPLEX_NUM 0 CACHE STRING "Address pin/Multiplexing type enum")
set(DEFINE_POWER_DIVISOR 1 CACHE STRING "Divisor for lowering average power. Requires bulk energy storage to smooth peak current")
set(DEFINE_USE_CIE1931 1 CACHE STRING "Use CIE1931 color temperature mapping table")

# create disassembly with source
function(pico_add_dis_output2 TARGET)
    add_custom_command(TARGET ${TARGET} POST_BUILD
        COMMAND ${CMAKE_OBJDUMP} -S $<TARGET_FILE:${TARGET}> >$<IF:$<BOOL:$<TARGET_PROPERTY:${TARGET},OUTPUT_NAME>>,$<TARGET_PROPERTY:${TARGET},OUTPUT_NAME>,$<TARGET_PROPERTY:${TARGET},NAME>>.dis2)
endfunction()

add_compile_definitions(
    PICO_HEAP_SIZE=2048
	PICO_XOSC_STARTUP_DELAY_MULTIPLIER=64
)

add_compile_options(
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

add_subdirectory(lib)

function(build_flavor TARGET)
    add_executable(led_${TARGET} ${TARGET}/main.cpp)
    add_subdirectory(${TARGET})
    include_directories(. ../include)

    target_compile_definitions(led_${TARGET} PUBLIC 
        DEFINE_MULTIPLEX=${DEFINE_MULTIPLEX} 
        DEFINE_MAX_RGB_LED_STEPS=${DEFINE_MAX_RGB_LED_STEPS} 
        DEFINE_MAX_REFRESH=${DEFINE_MAX_REFRESH}
        DEFINE_FPS=${DEFINE_FPS}
        DEFINE_COLUMNS=${DEFINE_COLUMNS}
        DEFINE_MULTIPLEX_NUM=${DEFINE_MULTIPLEX_NUM}
        DEFINE_POWER_DIVISOR=${DEFINE_POWER_DIVISOR}
        DEFINE_USE_CIE1931=${DEFINE_USE_CIE1931}
    )

    # create map/bin/hex file etc.
    pico_add_extra_outputs(led_${TARGET})
    pico_add_dis_output2(led_${TARGET})

    # pull in dependencies
    target_link_libraries(led_${TARGET} pico_stdlib)
endfunction()

function(build_defined TARGET)
    if (DEFINED ${TARGET})
        build_flavor(${TARGET})
    endif()
endfunction()

add_subdirectory(src)

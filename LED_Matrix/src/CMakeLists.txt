set(DEFINE_MULTIPLEX 32 CACHE STRING "Multiplex Scan of panel")
set(DEFINE_MAX_RGB_LED_STEPS 8000 CACHE STRING "Min LED const forward current in nA divided by min light current in nA")
set(DEFINE_MAX_REFRESH 3840 CACHE STRING "Max refresh of panel")
set(DEFINE_FPS 30 CACHE STRING "Frames per second")
set(DEFINE_COLUMNS 128 CACHE STRING "Chain length")
set(DEFINE_MULTIPLEX_NUM 0 CACHE STRING "Address pin/Multiplexing type enum")
set(DEFINE_POWER_DIVISOR 1 CACHE STRING "Divisor for lowering average power. Requires bulk energy storage to smooth peak current")
set(DEFINE_USE_CIE1931 1 CACHE STRING "Use CIE1931 color temperature mapping table")
set(DEFINE_SERIAL_CLOCK 15.625 CACHE STRING "HUB75 serial clock speed in MHz")

# create disassembly with source
function(pico_add_dis_output2 TARGET)
    add_custom_command(TARGET ${TARGET} POST_BUILD
        COMMAND ${CMAKE_OBJDUMP} -S $<TARGET_FILE:${TARGET}> >$<IF:$<BOOL:$<TARGET_PROPERTY:${TARGET},OUTPUT_NAME>>,$<TARGET_PROPERTY:${TARGET},OUTPUT_NAME>,$<TARGET_PROPERTY:${TARGET},NAME>>.dis2)
endfunction()

function(build_flavor TARGET)
    add_executable(led_${TARGET} ${TARGET}/main.cpp)
    add_subdirectory(${TARGET})
    include_directories(. ../include)

    # create map/bin/hex file etc.
    pico_add_extra_outputs(led_${TARGET})
    pico_add_dis_output2(led_${TARGET})
endfunction()

function(build_defined TARGET)
    if (DEFINED ${TARGET})
        build_flavor(${TARGET})
    endif()
endfunction()


build_defined(app1)
build_defined(app2)
build_defined(app3)

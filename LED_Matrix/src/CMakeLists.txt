function(build_flavor TARGET APP)
    add_executable(led_${TARGET} 
        ./main.cpp
        ../lib/Serial/serial_${APP}/isr.cpp
        ../lib/Serial/serial_${APP}/serial.cpp
    )

    target_include_directories(led_${TARGET} PRIVATE
        ../include
    )

    target_compile_options(led_${TARGET} PRIVATE 
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

    target_compile_definitions(led_${TARGET} PRIVATE 
        PICO_HEAP_SIZE=2048
        PICO_XOSC_STARTUP_DELAY_MULTIPLIER=64
        DEFINE_MULTIPLEX=${${TARGET}_DEFINE_MULTIPLEX} 
        DEFINE_MAX_RGB_LED_STEPS=${${TARGET}_DEFINE_MAX_RGB_LED_STEPS} 
        DEFINE_MAX_REFRESH=${${TARGET}_DEFINE_MAX_REFRESH}    
        DEFINE_FPS=${${TARGET}_DEFINE_FPS}   
        DEFINE_COLUMNS=${${TARGET}_DEFINE_COLUMNS}
        DEFINE_SERIAL_CLOCK=${${TARGET}_DEFINE_SERIAL_CLOCK}
        DEFINE_BLANK_TIME=${${TARGET}_DEFINE_BLANK_TIME}
        DEFINE_RED_GAIN=${${TARGET}_DEFINE_RED_GAIN}
        DEFINE_GREEN_GAIN=${${TARGET}_DEFINE_GREEN_GAIN}
        DEFINE_BLUE_GAIN=${${TARGET}_DEFINE_BLUE_GAIN}
        DEFINE_GCLK=${${TARGET}_DEFINE_GCLK}
        DEFINE_SERIAL_UART_BAUD=${${TARGET}_DEFINE_SERIAL_UART_BAUD}
        DEFINE_SERIAL_RGB_TYPE=${${TARGET}_DEFINE_SERIAL_RGB_TYPE}
    )

    pico_set_binary_type(led_${TARGET} copy_to_ram)

    # enable usb output, disable uart output
    pico_enable_stdio_usb(led_${TARGET} 0)
    pico_enable_stdio_uart(led_${TARGET} 1)
    
    target_link_libraries(led_${TARGET} 
        pico_runtime
        pico_multicore
        led_${${TARGET}_DEFINE_ALGORITHM}
        led_multiplex_${${TARGET}_DEFINE_MULTIPLEX_NAME}
        serial_${APP}
    )

    target_link_options(led_${TARGET} 
        PRIVATE "LINKER:--print-memory-usage"
    )

    # create map/bin/hex file etc.
    pico_add_extra_outputs(led_${TARGET})
    pico_add_dis_output2(led_${TARGET})
endfunction()

foreach(APP IN LISTS APPS)
    build_flavor(${APP} ${${APP}_APP})
endforeach()

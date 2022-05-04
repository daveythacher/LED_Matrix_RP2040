target_sources(led_${TARGET} PRIVATE
    serial.cpp
    isr.cpp
)

target_include_directories(led_${TARGET} PRIVATE
    ../../include
    ../../include/FreeRTOS
    ../../lib/FreeRTOS/FreeRTOS-Kernel/include 
    ../../lib/FreeRTOS/FreeRTOS-Kernel/portable/GCC/ARM_CM0
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
    DEFINE_MULTIPLEX=${DEFINE_MULTIPLEX} 
    DEFINE_MAX_RGB_LED_STEPS=${DEFINE_MAX_RGB_LED_STEPS} 
    DEFINE_MAX_REFRESH=${DEFINE_MAX_REFRESH}
    DEFINE_FPS=${DEFINE_FPS}
    DEFINE_COLUMNS=${DEFINE_COLUMNS}
    DEFINE_MULTIPLEX_NUM=${DEFINE_MULTIPLEX_NUM}
    DEFINE_POWER_DIVISOR=${DEFINE_POWER_DIVISOR}
    DEFINE_USE_CIE1931=${DEFINE_USE_CIE1931}
    DEFINE_SERIAL_CLOCK=${DEFINE_SERIAL_CLOCK}
    DEFINE_BLANK_TIME=${DEFINE_BLANK_TIME}
)

pico_set_binary_type(led_${TARGET} copy_to_ram)

# enable usb output, disable uart output
pico_enable_stdio_usb(led_${TARGET} 1)
pico_enable_stdio_uart(led_${TARGET} 0)
    
target_link_libraries(led_${TARGET} 
    pico_stdlib
    pico_multicore
    led_SPWM
    FreeRTOS
)

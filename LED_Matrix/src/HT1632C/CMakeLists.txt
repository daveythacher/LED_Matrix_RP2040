target_sources(led_${TARGET} PRIVATE
    serial.cpp
    isr.cpp
    matrix.cpp
)

add_subdirectory(../../lib/serial_spi "lib/serial_spi")

target_include_directories(led_${TARGET} PRIVATE
    .
    ../../include
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
    DEFINE_COLUMNS=${DEFINE_COLUMNS}
    DEFINE_POWER_DIVISOR=${DEFINE_POWER_DIVISOR}
    DEFINE_SERIAL_CLOCK=${DEFINE_SERIAL_CLOCK}
)

pico_set_binary_type(led_${TARGET} copy_to_ram)

# enable usb output, disable uart output
pico_enable_stdio_usb(led_${TARGET} 1)
pico_enable_stdio_uart(led_${TARGET} 0)
    
target_link_libraries(led_${TARGET} 
    pico_stdlib
    pico_multicore
    hardware_dma
    hardware_pio
    hardware_timer
    serial_spi
)

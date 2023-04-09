add_library(serial_usb)

target_sources(serial_usb PRIVATE
    dev_lowlevel.c
)

target_include_directories(serial_usb PRIVATE
    ../../../include
)

target_compile_options(serial_usb PRIVATE 
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

# Use caution here!
target_link_libraries(serial_usb PRIVATE 
    pico_stdlib
    hardware_resets
    hardware_irq
    hardware_dma        # Cannot use full API!
)

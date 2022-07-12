add_library(serial_uart)

target_sources(serial_uart PRIVATE
    serial_uart.cpp
)

target_include_directories(serial_uart PRIVATE
    ../../include
)

target_compile_options(serial_uart PRIVATE 
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
target_link_libraries(serial_uart PRIVATE 
    hardware_gpio
    hardware_irq
    hardware_uart
    hardware_dma        # Cannot use full API!
    hardware_resets
    hardware_clocks
)

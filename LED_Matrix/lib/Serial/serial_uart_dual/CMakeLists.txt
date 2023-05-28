add_library(serial_uart_dual)

target_sources(serial_uart_dual PRIVATE
    serial_uart_dual.cpp
)

target_include_directories(serial_uart_dual PRIVATE
    ../../../include
)

target_compile_options(serial_uart_dual PRIVATE 
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
target_link_libraries(serial_uart_dual PRIVATE 
    hardware_gpio
    hardware_irq
    hardware_uart
    hardware_dma        # Cannot use full API!
    hardware_timer
    hardware_resets
    hardware_clocks
)

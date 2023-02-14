add_library(serial_pmp)

target_sources(serial_pmp PRIVATE
    serial_pmp.cpp
)

target_include_directories(serial_pmp PRIVATE
    ../../../include
)

target_compile_options(serial_pmp PRIVATE 
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
target_link_libraries(serial_pmp PRIVATE 
    hardware_gpio
    hardware_irq
    hardware_pio
    hardware_dma        # Cannot use full API!
    hardware_resets
    hardware_clocks
)

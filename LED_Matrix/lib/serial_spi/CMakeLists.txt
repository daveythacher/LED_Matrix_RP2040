add_library(serial_spi)

target_sources(serial_spi PRIVATE
    serial_spi.cpp
)

target_include_directories(serial_spi PRIVATE
    ../../include
)

target_compile_options(serial_spi PRIVATE 
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
target_link_libraries(serial_spi PRIVATE 
    hardware_gpio
    hardware_irq
    hardware_spi
    hardware_dma        # Cannot use full API!
    hardware_resets
    hardware_clocks
)

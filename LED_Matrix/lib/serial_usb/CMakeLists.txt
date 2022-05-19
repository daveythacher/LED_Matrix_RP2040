add_library(serial_usb INTERFACE)

target_sources(serial_usb INTERFACE
    dev_lowlevel.c
)

target_include_directories(serial_usb INTERFACE
    ../../include
)

target_link_libraries(serial_usb INTERFACE
    pico_stdlib
    hardware_resets
    hardware_irq
)

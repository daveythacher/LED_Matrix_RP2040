# This could possibly be a static library
add_library(led_PIO_SM INTERFACE)

# TODO: Clean this up
target_sources(led_PIO_SM INTERFACE
    SM.cpp
)

target_link_libraries(led_PIO_SM INTERFACE
    hardware_gpio
)

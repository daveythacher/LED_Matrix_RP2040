# This could possibly be a static library
add_library(led_system INTERFACE)

target_sources(led_system INTERFACE
    machine.cpp
)
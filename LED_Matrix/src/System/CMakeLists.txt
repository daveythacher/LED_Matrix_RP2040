# This could possibly be a static library
add_library(led_system INTERFACE)

target_sources(led_system INTERFACE
    machine.cpp
    Watchdog.cpp
    IPC.cpp
)

target_link_libraries(led_system INTERFACE
    hardware_watchdog
    pico_util
)

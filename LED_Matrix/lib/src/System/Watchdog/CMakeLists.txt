# This could possibly be a static library
add_library(led_system_watchdog INTERFACE)

target_sources(led_system_watchdog INTERFACE
    Watchdog.cpp
)

target_link_libraries(led_system_watchdog INTERFACE 
    hardware_watchdog
)
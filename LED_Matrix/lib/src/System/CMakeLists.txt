add_subdirectory(Watchdog)

# This could possibly be a static library
add_library(led_system INTERFACE)

target_sources(led_system INTERFACE
    machine.cpp
)

target_link_libraries(led_system INTERFACE
    led_system_watchdog
)
add_library(led_debug INTERFACE)

target_sources(led_debug INTERFACE
    debug.c
)

target_include_directories(led_debug INTERFACE
    ../../include
)

target_link_libraries(led_debug INTERFACE
    pico_stdio
)

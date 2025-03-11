# This could possibly be a static library
add_library(led_Frame INTERFACE)

target_sources(led_Frame INTERFACE
    Frame.cpp
)

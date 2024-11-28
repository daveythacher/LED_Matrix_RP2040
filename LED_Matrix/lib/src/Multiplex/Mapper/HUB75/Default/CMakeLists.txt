# This could possibly be a static library
add_library(led_multiplex_mapper_HUB75_Default INTERFACE)

target_sources(led_multiplex_mapper_HUB75_Default INTERFACE
    Default.cpp
)

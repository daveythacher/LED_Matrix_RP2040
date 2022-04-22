add_library(led_multiplex INTERFACE)
target_sources(led_multiplex INTERFACE
    Multiplex.cpp
    Decoder.cpp
    Direct.cpp
)
target_link_libraries(led_multiplex INTERFACE pico_stdlib)

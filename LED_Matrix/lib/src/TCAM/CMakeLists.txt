# This could possibly be a static library
add_library(led_TCAM INTERFACE)

target_sources(led_TCAM INTERFACE
    TCAM.cpp
)

target_link_libraries(led_TCAM INTERFACE 
    led_SIMD
)

# This could possibly be a static library
add_library(led_SIMD INTERFACE)

target_sources(led_SIMD INTERFACE
    SIMD_QUARTER.cpp
    SIMD_SINGLE.cpp
    SIMD_HUB75.cpp
)

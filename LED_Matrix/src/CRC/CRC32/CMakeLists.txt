# This could possibly be a static library
add_library(led_crc_crc32 INTERFACE)

target_sources(led_crc_crc32 INTERFACE
    CRC32.cpp
)

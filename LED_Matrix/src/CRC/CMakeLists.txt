add_subdirectory(CRC32)

add_library(led_crc INTERFACE)

target_link_libraries(led_crc INTERFACE
    led_crc_crc32
)

add_subdirectory(Program)

add_library(led_pio INTERFACE)

target_link_libraries(led_pio INTERFACE
    led_PIO_Program
)
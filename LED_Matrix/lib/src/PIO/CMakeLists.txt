add_subdirectory(Program)

add_library(led_PIO INTERFACE)

target_link_libraries(led_PIO INTERFACE
    led_PIO_Program
)
add_subdirectory(Program)
add_subdirectory(SM)

# This could possibly be a static library
add_library(led_PIO INTERFACE)

target_link_libraries(led_PIO INTERFACE
    led_PIO_Program
    led_PIO_SM
)
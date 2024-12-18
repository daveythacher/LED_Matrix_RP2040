add_subdirectory(HUB75)
add_subdirectory(Test)

# Since we use preprocessor we have to use interface library
#   Optimization likely destroys any point in making this an actual lib

add_library(led_Matrix INTERFACE)

target_sources(led_Matrix INTERFACE
    Matrix.cpp
    Buffer.cpp
)

target_link_libraries(led_Matrix INTERFACE
    led_HUB75_PWM
    led_HUB75_BCM
    led_Test_Test
)
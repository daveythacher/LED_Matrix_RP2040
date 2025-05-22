add_subdirectory(BUS8)

# Since we use preprocessor we have to use interface library
#   Optimization likely destroys any point in making this an actual lib

add_library(led_matrix INTERFACE)

target_sources(led_matrix INTERFACE
    Factory.cpp
)

target_link_libraries(led_matrix INTERFACE
    led_BUS8_PWM
)
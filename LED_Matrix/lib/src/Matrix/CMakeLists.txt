add_subdirectory(Mono)

# Since we use preprocessor we have to use interface library
#   Optimization likely destroys any point in making this an actual lib

add_library(led_Matrix INTERFACE)

target_sources(led_Matrix INTERFACE
    Factory.cpp
)

target_link_libraries(led_Matrix INTERFACE
    led_HUB75_PWM
    #TODO: led_HUB75_BCM
)
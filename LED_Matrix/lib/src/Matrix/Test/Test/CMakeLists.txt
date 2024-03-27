# Since we use preprocessor we have to use interface library
#   Optimization likely destroys any point in making this an actual lib

add_library(led_Test_Test INTERFACE)

target_sources(led_Test_Test INTERFACE
    matrix.cpp
    worker.cpp
    loafer.cpp
)

target_link_libraries(led_Test_Test INTERFACE
    pico_multicore
    hardware_dma
    hardware_pio
    hardware_timer
)

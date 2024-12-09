# Since we use preprocessor we have to use interface library
#   Optimization likely destroys any point in making this an actual lib

add_library(led_HUB75_BCM INTERFACE)

target_sources(led_HUB75_BCM INTERFACE
    BCM_Matrix.cpp
    BCM_Worker.cpp
    Buffer.cpp
    calculator.cpp
    Programs.cpp
)

target_link_libraries(led_HUB75_BCM INTERFACE
    pico_multicore
    hardware_dma
    hardware_pio
    led_PIO
    led_multiplex_${DEFINE_MULTIPLEX_FOLDER}_${DEFINE_MULTIPLEX_ALGORITHM}
)

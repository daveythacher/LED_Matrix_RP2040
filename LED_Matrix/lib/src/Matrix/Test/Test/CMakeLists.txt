# Since we use preprocessor we have to use interface library
#   Optimization likely destroys any point in making this an actual lib

add_library(led_${DEFINE_MATRIX_FOLDER}_Test INTERFACE)

target_sources(led_${DEFINE_MATRIX_FOLDER}_Test INTERFACE
    matrix.cpp
    worker.cpp
    loafer.cpp
)

target_link_libraries(led_${DEFINE_MATRIX_FOLDER}_Test INTERFACE
    pico_multicore
    hardware_dma
    hardware_pio
    hardware_timer
)

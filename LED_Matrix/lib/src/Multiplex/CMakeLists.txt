add_subdirectory(BUS8)

# Since we use preprocessor we have to use interface library
#   Optimization likely destroys any point in making this an actual lib

add_library(led_multiplex INTERFACE)

target_sources(led_multiplex INTERFACE
    Multiplex.cpp
)

target_link_libraries(led_multiplex INTERFACE
    led_multiplex_${DEFINE_MULTIPLEX_FOLDER}_${DEFINE_MULTIPLEX_ALGORITHM}
)
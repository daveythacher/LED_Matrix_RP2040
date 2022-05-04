add_library(led_multiplex)

target_sources(led_multiplex PRIVATE
    Multiplex.cpp
    Decoder.cpp
    Direct.cpp
)

target_include_directories(led_multiplex PRIVATE
    ../../include
)

target_compile_options(led_multiplex PRIVATE 
    $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
    -fno-exceptions 
    -fno-check-new 
    $<$<COMPILE_LANGUAGE:CXX>:-fno-enforce-eh-specs>
    -g 
    -ffunction-sections 
    -fdata-sections 
    -O3
    -funroll-loops 
    -Werror 
    -Wall
)

# Use caution here!
target_link_libraries(led_multiplex PRIVATE 
    pico_stdlib
)

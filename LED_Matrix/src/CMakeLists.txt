function(build_flavor TARGET)
    add_executable(led_${TARGET} ${TARGET}/main.cpp)
    add_subdirectory(${TARGET})

    # create map/bin/hex file etc.
    pico_add_extra_outputs(led_${TARGET})
    pico_add_dis_output2(led_${TARGET})
endfunction()

function(build_defined TARGET)
    if (DEFINED ${TARGET})
        build_flavor(${TARGET})
    endif()
endfunction()


build_defined(app1)
build_defined(app2)
build_defined(app3)

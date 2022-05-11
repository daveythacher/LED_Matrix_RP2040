function(build_flavor TARGET APP)
    add_executable(led_${TARGET} ${APP}/main.cpp)
    add_subdirectory(${APP} ${TARGET})

    # create map/bin/hex file etc.
    pico_add_extra_outputs(led_${TARGET})
    pico_add_dis_output2(led_${TARGET})
endfunction()

foreach(APP IN LISTS APPS)
    build_flavor(${APP} ${${APP}_APP})
endforeach()

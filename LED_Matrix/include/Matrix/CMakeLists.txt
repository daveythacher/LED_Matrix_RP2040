add_subdirectory(BCM)
add_subdirectory(PWM)
add_subdirectory(S_PWM)
add_subdirectory(S_BCM)

configure_file(config.h.in config.h @ONLY)
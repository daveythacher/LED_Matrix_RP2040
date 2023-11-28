add_subdirectory(BCM)
add_subdirectory(PWM)
add_subdirectory(TLC5958)
add_subdirectory(TLC5946)

configure_file(config.h.in config.h @ONLY)
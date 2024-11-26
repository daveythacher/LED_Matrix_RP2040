# This could possibly be a static library
add_library(led_PIO INTERFACE)

# TODO: Clean this up
target_sources(led_PIO INTERFACE
    Program.cpp
    ASM/ASM.cpp
    ASM/NOP/NOP.cpp
    ASM/OUT/OUT.cpp
    ASM/PULL/PULL.cpp
    ASM/JMP/JMP.cpp
    ASM/IRQ/IRQ.cpp
    ASM/WAIT/WAIT.cpp
)

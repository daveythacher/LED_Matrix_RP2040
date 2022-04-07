#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "config.h"

extern void work();
extern void matrix_start();
extern void loop();

int main() {
    matrix_start();
    multicore_launch_core1(work);
    loop();
}

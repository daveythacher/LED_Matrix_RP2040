/* 
 * File:   main.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "pico/stdio_usb.h"
#include "pico/multicore.h"
#include "hardware/watchdog.h"
#include "hardware/timer.h"
#include "Matrix/matrix.h"
#include "Serial/serial.h"
#include "ISR/isr.h"

static void __not_in_flash_func(loop)() {
    while (1) {
        serial_task();
        watchdog_update();          // We are only interested in protecting core 0
    }
}

int main() {
    stdio_usb_init();
    busy_wait_ms(1000);             // Make time (1 second) for picotool before system loads
    watchdog_enable(100, false);    // Make sure picotool can never be disconnected
    matrix_start();
    serial_start();
    isr_start();
    loop();
}


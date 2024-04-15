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

static void __not_in_flash_func(loop_core0)() {
    while (1) {
        Serial::task();             // Uses blocking pushes to FIFO.
        watchdog_update();          // We are only interested in protecting core 0
    }
}

static void __not_in_flash_func(loop_core1)() {
    Matrix::start();                // Note ISR logic should be detected by watchdog. (FIFO will fill up and Matrix::Worker::work will stall.)
    APP::isr_start_core1();         // Matrix ISRs are allocated to this core. (These will slow the FIFO consumption rate.)

    while (1) {
        Matrix::Worker::work();     // Note is capable of stalling the watchdog via FIFO consumption rate. (Reduce OPs if need be.)
    }
}

int main() {
    stdio_usb_init();               // Enable picotool to call bootsel mode
    busy_wait_ms(1000);             // Make time (1 second) for picotool before system loads
    watchdog_enable(1, false);      // Make sure picotool can never be disconnected

    Serial::start();
    multicore_launch_core1(loop_core1);
    loop_core0();
}

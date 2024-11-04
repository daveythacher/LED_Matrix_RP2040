/* 
 * File:   main.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "pico/multicore.h"
#include "hardware/watchdog.h"
#include "Matrix/matrix.h"
#include "Serial/Node/Control/serial.h"
#include "Serial/Node/Data/serial.h"
#include "Serial/Protocol/serial.h"
#include "ISR/isr.h"

static void __not_in_flash_func(loop_core0)() {
    while (1) {
        // Uses blocking pushes to FIFO.
        Serial::Node::Control::task();
        Serial::Node::Data::task();
        Serial::Protocol::task();

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
    watchdog_enable(1, false);      // Make sure picotool can never be disconnected

    Serial::Node::Control::start();
    Serial::Node::Data::start();
    Serial::Protocol::start();
    multicore_launch_core1(loop_core1);
    loop_core0();
}

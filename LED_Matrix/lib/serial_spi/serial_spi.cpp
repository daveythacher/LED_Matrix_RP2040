/* 
 * File:   serial_spi.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#include <stdint.h>
#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "hardware/dma.h"
#include "serial_spi/serial_spi.h"

static serial_spi_callback func;

void serial_spi_start(serial_spi_callback callback) {
    func = callback;
    // TODO: 
}

void serial_spi_isr() {
    // TODO:
}


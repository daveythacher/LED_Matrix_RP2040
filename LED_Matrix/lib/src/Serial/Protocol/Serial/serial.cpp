/* 
 * File:   serial.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "hardware/watchdog.h"
#include "Serial/Protocol/serial.h"
#include "Serial/Protocol/Serial/internal.h"
#include "Serial/Protocol/Serial/Command/Command.h"
#include "Serial/Protocol/Serial/Command/filter.h"
#include "Serial/Protocol/Serial/control_node.h"

namespace Serial::Protocol {
    void start() {
        // Setup TCAM rules
        Serial::Protocol::DATA_NODE::filter::filter_setup();
    }

    // Warning host is required to obey flow control and handle bus recovery
    void __not_in_flash_func(task)() {
/*        static uint64_t time = time_us_64();
        Serial::UART::internal::STATUS status;*/

        Serial::Protocol::CONTROL_NODE::control_node();
        //status = Serial::UART::DATA_NODE::data_node();
        Serial::Protocol::DATA_NODE::Command::data_node();

        // TODO: Fix this (API for send_status?)
/*
        // Do not block, let flow control do it's thing
        if ((time_us_64() - time) >= (((10 * sizeof(Serial::UART::internal::Status_Message) * 1000000) / Serial::UART::SERIAL_UART_BAUD) + 1)) {
            Serial::UART::internal::send_status(status);
            time = time_us_64();
        }
*/
    }
}
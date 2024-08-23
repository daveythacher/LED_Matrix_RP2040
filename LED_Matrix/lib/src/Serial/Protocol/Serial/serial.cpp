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
#include "Serial/Node/data.h"

namespace Serial::Protocol {
    void start() {
        // Setup TCAM rules
        Serial::Protocol::DATA_NODE::filter::filter_setup();
    }

    // Warning host is required to obey flow control and handle bus recovery
    void __not_in_flash_func(task)() {
        static uint64_t time = time_us_64();
        Serial::Protocol::internal::STATUS status;

        Serial::Protocol::CONTROL_NODE::control_node();
        status = Serial::Protocol::DATA_NODE::Command::data_node();

        // Do not block, let flow control do it's thing
        if ((time_us_64() - time) >= (Serial::Node::Data::get_packet_time_us(sizeof(Serial::Protocol::internal::Status_Message)) + 1)) {
            Serial::Protocol::internal::send_status(status);
            time = time_us_64();
        }
    }
}
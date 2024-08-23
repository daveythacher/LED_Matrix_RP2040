/* 
 * File:   ID.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "pico/platform.h"
#include "Serial/Protocol/Serial/Command/Data/ID/ID.h"
#include "Serial/Protocol/Serial/control_node.h"
#include "System/machine.h"

namespace Serial::Protocol::DATA_NODE {
    void __not_in_flash_func(ID::process_command_internal)() {
        state_data = DATA_STATES::PAYLOAD;
        time = time_us_64();
        status = Serial::Protocol::internal::STATUS::ACTIVE_0;
        index = 0;
    }

    void __not_in_flash_func(ID::process_payload_internal)() {
        get_data(data.bytes, 1, true);

        if (index == 1) {
            state_data = DATA_STATES::CHECKSUM_DELIMITER_PROCESS;
            time = time_us_64();
            index = 0;
            status = Serial::Protocol::internal::STATUS::ACTIVE_1;
        }
    }

    void __not_in_flash_func(ID::process_frame_internal)() {
        // Future: Look into parity
        if (ntohl(data.data[0]) == ~checksum) {
            Serial::Protocol::CONTROL_NODE::set_id(data.bytes[0]);

            // We do not need to wait for ready.
            idle_num = (idle_num + 1) % 2;
            state_data = DATA_STATES::SETUP;
        }
    }

    void __not_in_flash_func(ID::process_internal)(Serial::packet *buf, uint16_t len) {
        // Do nothing
    }
}
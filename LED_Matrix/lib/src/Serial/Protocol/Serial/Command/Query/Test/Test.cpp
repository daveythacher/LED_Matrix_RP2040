/* 
 * File:   Test.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "pico/platform.h"
#include "Serial/Protocol/Serial/Command/Query/Test/Test.h"
#include "System/machine.h"

namespace Serial::Protocol::DATA_NODE {
    void __not_in_flash_func(Test::process_command_internal)() {
        state_data = DATA_STATES::PAYLOAD;
        update_time();
        status = Serial::Protocol::internal::STATUS::ACTIVE_0;
    }

    void __not_in_flash_func(Test::process_payload_internal)() {
        if (true) { // TODO: Get data?
            state_data = DATA_STATES::CHECKSUM_DELIMITER_PROCESS;
            update_time();
            status = Serial::Protocol::internal::STATUS::ACTIVE_1;
        }
    }

    void __not_in_flash_func(Test::process_frame_internal)() {
        // TODO: Get data?

        if (ntohl(data.l[0]) == ~checksum) {
            // TODO: Fill in the response packet
            state_data = DATA_STATES::READY_RESPONSE;
            status = Serial::Protocol::internal::STATUS::READY;
            update_time();
            //acknowledge = false;
        }
    }

    void __not_in_flash_func(Test::process_internal)(uint8_t *buf, uint16_t len) {
        // Do nothing?
    }
}
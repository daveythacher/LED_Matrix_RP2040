/* 
 * File:   Data.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "pico/platform.h"
#include "Serial/Protocol/Serial/Command/Data/Data/Data.h"
#include "System/machine.h"

namespace Serial::Protocol::DATA_NODE {
    void __not_in_flash_func(Data::process_command_internal)() {
        state_data = DATA_STATES::PAYLOAD;
        time = time_us_64();
        status = Serial::Protocol::internal::STATUS::ACTIVE_0;
        index = 0;
        trigger = false;
    }

    void __not_in_flash_func(Data::process_payload_internal)() {
        get_data(buf->raw, len, true);

        if (len == index) {
            state_data = DATA_STATES::CHECKSUM_DELIMITER_PROCESS;
            time = time_us_64();
            index = 0;
            status = Serial::Protocol::internal::STATUS::ACTIVE_1;
            trigger = false;
        }

    }

    void __not_in_flash_func(Data::process_frame_internal)() {
        // Future: Look into parity
        if (ntohl(data.data[0]) == ~checksum) {
            state_data = DATA_STATES::READY;
            time = time_us_64();
            status = Serial::Protocol::internal::STATUS::READY;
            trigger = false;
        }
    }

    void __not_in_flash_func(Data::process_internal)(Serial::packet *buf, uint16_t len) {
        Serial::Protocol::internal::process(buf, len);
    }
}
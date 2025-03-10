/* 
 * File:   Buffer.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "pico/platform.h"
#include "Serial/Protocol/Serial/Command/Data/Buffer/Buffer.h"
#include "System/machine.h"

namespace Serial::Protocol::DATA_NODE {
    void __not_in_flash_func(Buffer::process_command_internal)() {
        state_data = DATA_STATES::PAYLOAD;
        update_time();
        status = Serial::Protocol::internal::STATUS::ACTIVE_0;
        clear_trigger();
    }

    void __not_in_flash_func(Buffer::process_payload_internal)() {
        if (get_data(buf, len, true)) {
            state_data = DATA_STATES::CHECKSUM_DELIMITER_PROCESS;
            update_time();
            status = Serial::Protocol::internal::STATUS::ACTIVE_1;
            clear_trigger();
        }

    }

    void __not_in_flash_func(Buffer::process_frame_internal)() {
        // Future: Look into parity
        if (ntohl(data.l[0]) == ~checksum) {
            state_data = DATA_STATES::READY;
            update_time();
            status = Serial::Protocol::internal::STATUS::READY;
            clear_trigger();
        }
    }

    void __not_in_flash_func(Buffer::process_internal)(uint8_t *buf, uint16_t len) {
        // TODO:
        //Matrix::Packet *p = new Matrix::Packet();
        //Serial::Protocol::internal::process(p);
    }
}
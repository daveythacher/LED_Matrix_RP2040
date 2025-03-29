/* 
 * File:   Raw_Data.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "pico/platform.h"
#include "Serial/Protocol/Serial/Command/Data/Raw_Data/Raw_Data.h"
#include "Matrix/Buffer.h"
#include "Matrix/Types.h"

namespace Serial::Protocol::DATA_NODE {
    void __not_in_flash_func(Raw_Data::process_command_internal)() {
        state_data = DATA_STATES::PAYLOAD;
        update_time();
        status = Serial::Protocol::internal::STATUS::ACTIVE_0;
        clear_trigger();
    }

    void __not_in_flash_func(Raw_Data::process_payload_internal)() {
        if (get_data(buf, len, false)) {
            state_data = DATA_STATES::CHECKSUM_DELIMITER_PROCESS;
            update_time();
            status = Serial::Protocol::internal::STATUS::ACTIVE_1;
            clear_trigger();
        }
    }

    void __not_in_flash_func(Raw_Data::process_frame_internal)() {
            state_data = DATA_STATES::READY;
            update_time();
            status = Serial::Protocol::internal::STATUS::READY;
            clear_trigger();
    }

    void __not_in_flash_func(Raw_Data::process_internal)(uint8_t *buf, uint16_t len) {
        // TODO:
        //Matrix::Buffer<Matrix::RGB24> *b = Matrix::Buffer<Matrix::RGB24>::create_buffer(8, 32);
        //Serial::Protocol::internal::process(b);
    }
}
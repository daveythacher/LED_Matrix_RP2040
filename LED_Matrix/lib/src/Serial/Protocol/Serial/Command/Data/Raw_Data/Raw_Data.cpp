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
        time = time_us_64();
        status = Serial::Protocol::internal::STATUS::ACTIVE_0;
        index = 0;
        trigger = false;
    }

    void __not_in_flash_func(Raw_Data::process_payload_internal)() {
        get_data(buf, len, false);

        if (len == index) {
            state_data = DATA_STATES::CHECKSUM_DELIMITER_PROCESS;
            time = time_us_64();
            index = 0;
            status = Serial::Protocol::internal::STATUS::ACTIVE_1;
            trigger = false;
        }
    }

    void __not_in_flash_func(Raw_Data::process_frame_internal)() {
            state_data = DATA_STATES::READY;
            time = time_us_64();
            status = Serial::Protocol::internal::STATUS::READY;
            trigger = false;
    }

    void __not_in_flash_func(Raw_Data::process_internal)(Serial::packet *buf, uint16_t len) {
        Matrix::Buffer<Matrix::RGB24> *b = new Matrix::Buffer<Matrix::RGB24>();
        Serial::Protocol::internal::process(b);
    }
}
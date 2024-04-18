/* 
 * File:   Status_Message.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "Serial/serial_uart/machine.h"
#include "Serial/serial_uart/internal.h"
#include "Serial/serial_uart/CRC.h"

namespace Serial::UART::internal {
    __not_in_flash_func(Status_Message::Status_Message)() {
        header = htonl(0xAAEEAAEE);
        cmd = 's';
        len = htons(4);
        delimiter = htonl(0xAEAEAEAE);
    }

    void __not_in_flash_func(Status_Message::set_status)(STATUS s) {
        switch (s) {
            case STATUS::IDLE_0:
                status = htonl(0);
                break;

            case STATUS::IDLE_1:
                status = htonl(1);
                break;

            case STATUS::ACTIVE_0:
                status = htonl(2);
                break;

            case STATUS::ACTIVE_1:
                status = htonl(3);
                break;

            case STATUS::READY:
                status = htonl(4);
                break;

            default:
                status = htonl(0xFFFFFFFF);
                break;
        }

        checksum = htonl(compute_checksum());
    }

    static inline uint32_t __not_in_flash_func(checksum_chunk)(uint32_t checksum, uint32_t v, uint8_t bits) {
        for (int i = 0; i < bits; i += 8)
            checksum = Serial::UART::CRC::crc32(checksum, (v >> i) & 0xFF);
        
        return checksum;
    }

    inline uint32_t __not_in_flash_func(Status_Message::compute_checksum)() {
        uint32_t checksum = 0xFFFFFFFF;

        checksum = checksum_chunk(checksum, header, 32);
        checksum = checksum_chunk(checksum, cmd, 8);
        checksum = checksum_chunk(checksum, len, 16);
        checksum = checksum_chunk(checksum, status, 32);

        return ~checksum;
    }
}
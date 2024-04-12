/* 
 * File:   internal.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef SERIAL_UART_INTERNAL_H
#define SERIAL_UART_INTERNAL_H
    
#include <stdint.h>
#include "pico/multicore.h"

namespace Serial::UART::internal {
    enum class STATUS {
        IDLE_0,
        IDLE_1,
        ACTIVE_0,
        ACTIVE_1,
        READY
    };

    struct Status_Message {
        public:
            Status_Message();

            void set_status(STATUS s);

            uint32_t header;
            uint8_t cmd;
            uint16_t len;
            uint32_t status;
            uint32_t checksum;
            uint32_t delimiter;

        private:
            uint32_t compute_checksum();
    };

    void process(uint16_t *buf, uint16_t len);
    void send_status(STATUS status);
    void send_message(Status_Message *message);

    // TODO: Consider moving this to look up table for performance
    //  Given the fact we are on microcontroller we likely cannot support buffers,
    //  unless we have proper hardware framing. (We do not currently use this.)
    inline uint32_t __not_in_flash_func(crc)(uint32_t crc, uint8_t data) {
        crc ^= data;

        for (int i = 0; i < 8; i++)
            crc = crc & 1 ? (crc >> 1) ^ 0x82F63B78 : crc >> 1;

        return crc;
    }
}

#endif

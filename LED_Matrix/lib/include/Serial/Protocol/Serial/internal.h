/* 
 * File:   internal.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef SERIAL_UART_INTERNAL_H
#define SERIAL_UART_INTERNAL_H
    
#include <stdint.h>
#include "pico/multicore.h"
#include "Serial/config.h"

namespace Serial::UART::internal {
    enum class STATUS {
        IDLE_0,
        IDLE_1,
        ACTIVE_0,
        ACTIVE_1,
        READY
    };

    // Should be less than 32 bytes in size
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

    void process(Serial::packet *buf, uint16_t len);
    void send_status(STATUS status);
    void send_message(Status_Message *message);
}

#endif

/* 
 * File:   types.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef SERIAL_UART_TYPES_H
#define SERIAL_UART_TYPES_H
    
#include "Serial/serial_uart/machine.h"

namespace Serial::UART {
    enum class DATA_STATES {
        SETUP,
        PREAMBLE_CMD_LEN,
        PAYLOAD,
        CHECKSUM_DELIMITER_PROCESS
    };

    enum class CONTROL_STATES {
        SETUP,
        LISTEN,
        TRIGGER
    };

    union random_type {
        uint8_t bytes[8];
        uint16_t shorts[4];
        uint32_t longs[2];
    };

    enum class STATUS {
        IDLE_0,
        IDLE_1,
        ACTIVE_0,
        ACTIVE_1
    };

    enum class COMMAND {
        DATA,
        RAW_DATA,
        SET_ID
    };

    struct Control_Message {
        uint32_t header;
        uint8_t cmd;
        uint16_t len;
        uint8_t id;
        uint8_t payload;
        uint32_t checksum;
        uint32_t delimiter;
    };

    struct Status_Message {
        Status_Message() {
            header = htonl(0xAAEEAAEE);
            cmd = 's';
            len = htons(4);
            delimiter = htonl(0xAEAEAEAE);
        }

        void set_status(STATUS s) {
            switch (s) {
                case STATUS::IDLE_0:
                    status = htonl(0);
                    // TODO: Checksum
                    checksum = htonl(0);
                    break;
                case STATUS::IDLE_1:
                    status = htonl(1);
                    // TODO: Checksum
                    checksum = htonl(0);
                    break;
                case STATUS::ACTIVE_0:
                    status = htonl(2);
                    // TODO: Checksum
                    checksum = htonl(0);
                    break;
                case STATUS::ACTIVE_1:
                    status = htonl(3);
                    // TODO: Checksum
                    checksum = htonl(0);
                    break;
                default:
                    status = htonl(0xFFFFFFFF);
                    // TODO: Checksum
                    checksum = htonl(0);
                    break;
            }
        }

        uint32_t header;
        uint8_t cmd;
        uint16_t len;
        uint32_t status;
        uint32_t checksum;
        uint32_t delimiter;
    };
}

#endif

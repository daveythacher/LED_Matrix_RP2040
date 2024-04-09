/* 
 * File:   data_node.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef SERIAL_UART_DATA_NODE_H
#define SERIAL_UART_DATA_NODE_H
    
#include "Serial/serial_uart/internal.h"

namespace Serial::UART::DATA_NODE {
    enum class DATA_STATES {
        SETUP,
        PREAMBLE_CMD_LEN,
        PAYLOAD,
        CHECKSUM_DELIMITER_PROCESS,
        READY
    };

    union random_type {
        uint8_t bytes[8];
        uint16_t shorts[4];
        uint32_t longs[2];
    };

    enum class COMMAND {
        DATA,
        RAW_DATA,
        SET_ID,
        BLANK
    };

    Serial::UART::internal::STATUS data_node();
    void trigger_processing();
    void reset();
}

#endif

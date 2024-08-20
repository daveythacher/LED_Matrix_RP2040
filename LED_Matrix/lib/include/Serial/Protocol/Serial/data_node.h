/* 
 * File:   data_node.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef SERIAL_UART_DATA_NODE_H
#define SERIAL_UART_DATA_NODE_H
    
#include "Serial/Protocol/Serial/internal.h"

namespace Serial::UART::DATA_NODE {
    enum class DATA_STATES {
        SETUP,
        PREAMBLE_CMD_LEN_T_MULTIPLEX_COLUMNS,
        PAYLOAD,
        CHECKSUM_DELIMITER_PROCESS,
        READY,
        READY_RESPONSE
    };

    enum class COMMAND {
        DATA,
        RAW_DATA,
        SET_ID,
        QUERY_TEST
    };

    void data_node_setup();
    Serial::UART::internal::STATUS data_node();
    void trigger_processing();
    void acknowledge_query();
    void reset();
}

#endif

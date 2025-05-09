/* 
 * File:   Test.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef SERIAL_PROTOCOL_SERIAL_COMMAND_QUERY_TEST_H
#define SERIAL_PROTOCOL_SERIAL_COMMAND_QUERY_TEST_H

#include "Serial/Protocol/Serial/Command/Command.h"

namespace Serial::Protocol::DATA_NODE {
    class Test : public Command {
        protected:
            void process_frame_internal();
            void process_command_internal();
            void process_payload_internal();
            void process_internal(uint8_t *buf, uint16_t len);
    };
}

#endif

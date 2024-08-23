/* 
 * File:   ID.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef SERIAL_PROTOCOL_SERIAL_COMMAND_DATA_ID_H
#define SERIAL_PROTOCOL_SERIAL_COMMAND_DATA_ID_H

#include "Serial/Protocol/Serial/Command/Command.h"

namespace Serial::Protocol::DATA_NODE {
    class ID : public Command {
        protected:
            void process_frame_internal();
            void process_command_internal();
            void process_payload_internal();
    };
}

#endif

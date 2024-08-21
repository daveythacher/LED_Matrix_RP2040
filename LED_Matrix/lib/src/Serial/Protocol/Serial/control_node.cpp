/* 
 * File:   control_node.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "pico/multicore.h"
#include "Serial/Protocol/Serial/control_node.h"
#include "Serial/Protocol/Serial/data_node.h"
#include "Serial/Node/control.h"
#include "System/machine.h"
#include "CRC/CRC.h"

namespace Serial::Protocol::CONTROL_NODE {
    static uint8_t id = 0;

    static bool get_message(Control_Message *msg, uint32_t *checksum);
    
    void __not_in_flash_func(control_node)() {
        static Control_Message message;
        static uint32_t checksum = 0xFFFFFFFF;

        // Never respond to control messages
        if (get_message(&message, &checksum)) {
            // Future: Look into parity
            if (message.header == ntohl(0xAAEEAAEE) &&
                    message.delimiter == ntohl(0xAEAEAEAE) &&
                    message.len == 1 &&
                    message.checksum == htonl(~checksum)) {

                switch (message.cmd) {
                    case 0:
                        if (message.id == 0 || message.id == id)
                            Serial::Protocol::DATA_NODE::trigger_processing();
                        break;

                    case 1:
                        if (message.id == 0 || message.id == id)
                            Serial::Protocol::DATA_NODE::reset();
                        break;

                    case 2:
                        if (message.id == id)
                            Serial::Protocol::DATA_NODE::acknowledge_query();
                        break;

                    default:
                        break;
                }
            }

            checksum = 0xFFFFFFFF;
        }
    }

    void __not_in_flash_func(set_id)(uint8_t num) {
        id = num;
    }

    bool __not_in_flash_func(get_message)(Control_Message *msg, uint32_t *checksum) {
        static uint32_t index = 0;      // Hopefully the compiler will clean this up
        static uint8_t state = 0;
        bool check_it = false;
        bool result = false;
        uint8_t c;

        if (Serial::Node::Control::isAvailable()) {
            c = Serial::Node::Control::getc();
            index++;

            switch (state) {
                case 0:     // Header
                    msg->header = c << (8 * (index - 1));

                    if (index == sizeof(uint32_t)) {
                        index = 0;
                        state++;
                        msg->header = ntohl(msg->header);
                    }
                    break;

                case 1:     // CMD
                    msg->cmd = c << (8 * (index - 1));
                    
                    if (index == sizeof(uint8_t)) {
                        index = 0;
                        state++;
                    }
                    break;

                case 2:     // len
                    msg->len = c << (8 * (index - 1));
                    
                    if (index == sizeof(uint16_t)) {
                        index = 0;
                        state++;
                        msg->len = ntohs(msg->len);
                    }
                    break;
                    
                case 3:     // id
                    msg->id = c << (8 * (index - 1));
                    
                    if (index == sizeof(uint8_t)) {
                        index = 0;
                        state++;
                    }
                    break;
                    
                case 4:     // checksum
                    msg->checksum = c << (8 * (index - 1));
                    check_it = false;
                    
                    if (index == sizeof(uint32_t)) {
                        index = 0;
                        state++;
                        msg->checksum = ntohl(msg->checksum);
                    }
                    break;
                    
                case 5:     // delimiter
                    msg->delimiter = c << (8 * (index - 1));
                    check_it = false;
                    
                    if (index == sizeof(uint32_t)) {
                        index = 0;
                        state = 0;
                        result = true;
                        msg->delimiter = ntohl(msg->delimiter);
                    }
                    break;

                default:
                    state = 0;
                    index = 0;
                    break;
            }

            if (check_it)
                *checksum = CRC::crc32(*checksum, c);
        }

        return result;;
    }
}
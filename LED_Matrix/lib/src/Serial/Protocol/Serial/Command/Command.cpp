/* 
 * File:   Command.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "Serial/Protocol/Serial/Command/Command.h"
#include "Concurrent/Thread/Thread.h"
#include "Serial/Node/data.h"
#include "System/machine.h"
#include "CRC/CRC.h"
using Serial::Protocol::internal::STATUS;

namespace Serial::Protocol::DATA_NODE {
    extern TCAM::Table<SIMD::SIMD<uint32_t, SIMD::SIMD_128>> data_filter;
}

namespace Serial::Protocol::DATA_NODE {
    Command::DATA_STATES Command::state_data = DATA_STATES::SETUP;
    uint8_t Command::idle_num = 0;
    uint32_t Command::index;
    Command::uint128_t Command::data;
    uint32_t Command::checksum;
    STATUS Command::status;
    bool Command::trigger;
    bool Command::acknowledge;
    uint64_t Command::time;
    Command *Command::ptr;
    
    void Command::data_node(void *) {
        // TODO: Init

        // Note we respond with an ack packet or nothing on a dedicated line.
        //  Ack packets can have data put into them.
        while (1) {
            // Look for frame preamble
            while (1) {
                static uint32_t preamble_word = 0;

                if (Serial::Node::Data::isAvailable()) {
                    preamble_word <<= 8;
                    preamble_word |= Serial::Node::Data::getc();

                    if (preamble_word == htonl(0xAAEEAAEE)) {
                        for (uint8_t i = 0; i < 4; i++) {
                            checksum = CRC::crc32(checksum, (preamble_word >> (8 * (3 - i))) & 0xFF);
                        }
                        break;
                    }
                }
                else {
                    Concurrent::Thread::Yield();
                }
            }

            // Find packet length
            while (1) {
                if (Serial::Node::Data::isAvailable()) {
                    len = Serial::Node::Data::getc();
                    break;
                }
                else {
                    Concurrent::Thread::Yield();
                }
            }

            // Get packet data
            while (1) {
                if (Serial::Node::Data::isAvailable()) {
                    // TODO: Capture data

                    if (--len == 0) {
                        break;
                    }
                }
                else {
                    Concurrent::Thread::Yield();
                }
            }
            
            // TODO: checksum and trailing preamble
        }
    }

    void __not_in_flash_func(Command::clear_trigger)() {
        trigger = false;
    }

    void __not_in_flash_func(Command::update_time)() {
        time = time_us_64();
    }

    void __not_in_flash_func(Command::trigger_processing)() {
        trigger = true;
    }

    void __not_in_flash_func(Command::acknowledge_query)() {
        acknowledge = true;
    }

    // Hack to hand singleton to dynamic object
    //  We do not know the dynamic object initially
    //  We will release back to singleton control
    void __not_in_flash_func(Command::callback)() {
        ptr = this;
        process_command_internal();
    }
}
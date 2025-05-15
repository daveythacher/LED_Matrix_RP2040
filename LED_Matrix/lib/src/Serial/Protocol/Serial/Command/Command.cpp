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

        // Note we respond with an ack or nothing on a dedicated line.
        while (1) {
            static uint32_t preamble_word = 0;
            static uint8_t counter = 0;

            // Look for frame preamble
            while (Serial::Node::Data::isAvailable()) {
                preamble_word <<= 8;
                preamble_word |= Serial::Node::Data::getc();

                if (preamble_word == htonl(0xAAEEAAEE)) {
                    for (uint8_t i = 0; i < 4; i++) {
                        checksum = CRC::crc32(checksum, (preamble_word >> (8 * (3 - i))) & 0xFF);
                    }
                    break;
                }
            }

            if (!Serial::Node::Data::isAvailable()) {
                Concurrent::Thread::Yield();
            }

            // Find packet length
            while (Serial::Node::Data::isAvailable()) {
                len <<= 8;
                len |= Serial::Node::Data::getc();
                ++counter;

                if (counter >= 2) {
                    len = ntohs(len);
                    counter = 0;
                    break;
                }
            }

            if (!Serial::Node::Data::isAvailable()) {
                Concurrent::Thread::Yield();
            }

            // Get packet data
            while (Serial::Node::Data::isAvailable()) {
                // TODO: Capture data

                if (--len == 0) {
                    break;
                }
            }

            if (!Serial::Node::Data::isAvailable()) {
                Concurrent::Thread::Yield();
            }
            
            // TODO: checksum and trailing preamble
        }
    }

    STATUS __not_in_flash_func(Command::data_node)() {

        // Currently we drop the frame and wait for the next valid header.
        //  Host app will do the right thing using status messages.
        switch (state_data) {
            case DATA_STATES::SETUP:
                checksum = 0xFFFFFFFF;
                state_data = DATA_STATES::HEADER;

                index = 0;
                clear_trigger();
                acknowledge = false;
                update_time();

                if (idle_num == 0)
                    status = STATUS::IDLE_0;
                else
                    status = STATUS::IDLE_1;
                break;

            // Host protocol should create bubble waiting for status after sending data.
            //  Half duplex like currently for simplicity. We should have the bandwidth.
            //  Host needs to be on the ball though. Performance loss is possible from OS!
            case DATA_STATES::PREAMBLE_CMD_LEN_T_MULTIPLEX_COLUMNS: // Host should see IDLE_0/1 to ACTIVE_0
                {
                    static uint32_t state = 0;
                    uint8_t sum[4];

                    // This is protected by the reset timer, but mistakes can lead to high error rates
                    switch (state) {
                        case 0: // Grab the header
                            if (get_data(data.b, 12, true)) {
                                state = 1;                      // Advances state (local)
                            }
                            break;

                        case 1: // Grab the checksum for the header, process header if it checks out
                            if (get_data(sum, 4, false)) {
                                uint32_t csum = (sum[3] << 24) + (sum[2] << 16) + (sum[1] << 8) + sum[0];
                                csum = ntohl(csum);

                                if (~checksum == csum) {
                                    state = 0;                  // Assume local will advance (reset for next iteration of global)
                                    checksum = 0xFFFFFFFF;
                                    SIMD::SIMD<uint32_t, SIMD::SIMD_128> temp;

                                    for (uint8_t index = 0; index < 4; index++)
                                        temp.set(data.l[index], index);
                                        
                                    // This calls process_command_internal
                                    if (data_filter.TCAM_process(&temp)) {   // Advances state (global)
                                        state = 2;              // Advances state (local)
                                    }
                                }
                                else {
                                    state = 2;                  // Advances state (local)
                                }
                            }
                            break;

                        case 2: // This exists to allow local to reset for next iteration
                        default:
                            state = 0;                          // Reset for next iteration of global
                            error();                            // Advances state (global)
                            break;
                    }
                }
                break;

            // Host protocol should create bubble waiting for status after sending data.
            //  Half duplex like currently for simplicity. We should have the bandwidth.
            //  Host needs to be on the ball though. Performance loss is possible from OS!
            case DATA_STATES::PAYLOAD:                              // Host should see ACTIVE_0 to ACTIVE_1
                if (ptr != nullptr) {
                    ptr->process_payload_internal();            // Advances state
                }
                else {
                    error();                                    // Advances state
                }
                break;

            // Host protocol should create bubble waiting for status after sending data.
            //  Half duplex like currently for simplicity. We should have the bandwidth.
            //  Host needs to be on the ball though. Performance loss is possible from OS!
            case DATA_STATES::CHECKSUM_DELIMITER_PROCESS:           // Host should see ACTIVE_1 to IDLE_1/0 or READY
                if (get_data(data.b, 8, false)) {                            
                    if (ntohl(data.l[1]) == 0xAEAEAEAE) {
                        if (ptr != nullptr) {
                            ptr->process_frame_internal();      // Advances state
                        }
                        else {
                            error();                            // Advances state
                        }
                    }
                    else {
                        error();                                // Advances state
                    }
                }
                break;

            // Host protocol should create bubble waiting for status after sending data.
            //  Half duplex like currently for simplicity. We should have the bandwidth.
            //  Host needs to be on the ball though. Performance loss is possible from OS!
            case DATA_STATES::READY:                                // Host should see READY to IDLE_1/0
                if (trigger) {
                    if (ptr != nullptr) {
                        ptr->process_internal(buf, len);
                        idle_num = (idle_num + 1) % 2;
                        state_data = DATA_STATES::SETUP;
                    }
                    else {
                        error();                                // Advances state
                    }
                }
                else {
                    // Wait for reset
                }
                break;
                
            // Host protocol should create bubble waiting for status containing response data.
            //  Half duplex like currently for simplicity.
            case DATA_STATES::READY_RESPONSE:
                if (acknowledge) {
                    idle_num = (idle_num + 1) % 2;
                    state_data = DATA_STATES::SETUP;
                }
                else {
                    // Wait for reset
                }
                break;
            
            case DATA_STATES::ERROR:
                // Wait for reset
                break;

            default:
                state_data = DATA_STATES::SETUP;
                break;
        }

        // Timeout after 1mS
        //  Data node should yield from timeout or watchdog.
        //      If the Data node is compromised, the device is offline.
        //          Malformed responses will DoS offline.
        //      If the Control node is compromised, the device will respond to local commands only.
        //      If the bootloader or watchdog fail to reset, the data node is compromised.
        if ((time_us_64() - time) >= 1000) {
            reset();
        }

        return status;
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

    void __not_in_flash_func(Command::reset)() {
        state_data = DATA_STATES::SETUP;
    }

    void __not_in_flash_func(Command::error)() {
        state_data = DATA_STATES::ERROR;
    }

    bool __not_in_flash_func(Command::get_data)(uint8_t *buf, uint16_t len, bool checksum) {
        while (index < len) {
            if (Serial::Node::Data::isAvailable()) {
                buf[index] = Serial::Node::Data::getc();

                if (checksum)
                    checksum = CRC::crc32(checksum, buf[index]);

                index++;
            }
            else
                break;
        }

        bool result = index == len;

        if (result) {
            index = 0;
        }

        return result;
    }

    // Hack to hand singleton to dynamic object
    //  We do not know the dynamic object initially
    //  We will release back to singleton control
    void __not_in_flash_func(Command::callback)() {
        ptr = this;
        process_command_internal();
    }
}
/* 
 * File:   Command.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "Serial/Protocol/Serial/Command/Command.h"
#include "Serial/Node/data.h"
#include "System/machine.h"
#include "CRC/CRC.h"
using Serial::Protocol::internal::STATUS;

namespace Serial::Protocol::DATA_NODE {
    Serial::packet *Command::buf = 0;
    uint16_t Command::len = 0;
    Command::DATA_STATES Command::state_data = DATA_STATES::SETUP;
    uint8_t Command::idle_num = 0;
    uint32_t Command::index;
    TCAM::TCAM_entry Command::data;
    uint32_t Command::checksum;
    STATUS Command::status;
    bool Command::trigger;
    bool Command::acknowledge;
    uint64_t Command::time;
    Command *Command::ptr = nullptr;
    
    STATUS __not_in_flash_func(Command::data_node)() {
        // Currently we drop the frame and wait for the next valid header.
        //  Host app will do the right thing using status messages.
        switch (state_data) {
            case DATA_STATES::SETUP:
                index = 0;
                trigger = false;
                acknowledge = false;
                checksum = 0xFFFFFFFF;
                Serial::Node::Data::callback(&buf);
                len = Serial::Node::Data::get_len();
                state_data = DATA_STATES::PREAMBLE_CMD_LEN_T_MULTIPLEX_COLUMNS;
                time = time_us_64();

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
                            get_data(data.bytes, 12, true);

                            if (index == 12) {
                                index = 0;
                                state = 1;                      // Advances state (local)
                            }
                            break;

                        case 1: // Grab the checksum for the header, process header if it checks out
                            get_data(sum, 4, false);

                            if (index == 4) {
                                uint32_t csum = (sum[3] << 24) + (sum[2] << 16) + (sum[1] << 8) + sum[0];
                                csum = ntohl(csum);

                                if (~checksum == csum) {
                                    ptr = nullptr;
                                    state = 0;                  // Assume local will advance (reset for next iteration of global)
                                    checksum = 0xFFFFFFFF;
                                        
                                    // This calls process_command_internal
                                    TCAM::TCAM_process(&data);  // Advances state (global)

                                    if (ptr == nullptr) {
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
                get_data(data.bytes, 8, false);

                if (index == 8) {
                    index = 0;
                            
                    if (ntohl(data.data[1]) == 0xAEAEAEAE) {
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

    void __not_in_flash_func(Command::get_data)(uint8_t *buf, uint16_t len, bool checksum) {
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
    }
}
/* 
 * File:   data_node.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "Serial/Protocol/Serial/data_node.h"
#include "Serial/Protocol/Serial/internal.h"
#include "Serial/Protocol/Serial/control_node.h"
#include "Serial/Node/data.h"
#include "System/machine.h"
#include "TCAM/tcam.h"
#include "CRC/CRC.h"
using Serial::Protocol::internal::STATUS;

namespace Serial::Protocol::DATA_NODE {
    static Serial::packet *buf = 0;
    static uint16_t len = 0;

    static DATA_STATES state_data = DATA_STATES::SETUP;
    static uint8_t idle_num = 0;
    static COMMAND command;
    static uint32_t index;
    static TCAM::TCAM_entry data;
    static uint32_t checksum;
    static STATUS status;
    static bool trigger;
    static bool acknowledge;
    static uint64_t time;

    static void get_data(uint8_t *buf, uint16_t len, bool checksum);
    static void process_payload();
    static void process_frame();

    static void process_data_command_d();
    static void process_data_command_r();
    static void process_control_command_i();
    static void process_query_request_t();

    void data_node_setup() {
        TCAM::TCAM_entry key;
        TCAM::TCAM_entry enable;

        // TCAM can covert 6-12 operations down to 3.
        //  The conditionals can be removed with AND down to 1.
        enable.data[0] = 0xFFFFFFFF;
        enable.data[1] = 0xFFFFFFFF;
        enable.data[2] = 0xFFFFFFFF;

        key.data[0] = htonl(0xAAEEAAEE);
        key.bytes[4] = 'd';
        key.bytes[5] = 'd';
        key.shorts[3] = htons(Serial::Node::Data::get_len());
        key.bytes[8] = sizeof(DEFINE_SERIAL_RGB_TYPE);
        key.bytes[9] = Matrix::MULTIPLEX;
        key.bytes[10] = Matrix::COLUMNS;
        key.bytes[11] = DEFINE_SERIAL_RGB_TYPE::id;
        while (!TCAM::TCAM_rule(0, key, enable, process_data_command_d));

        key.bytes[4] = 'r';
        while (!TCAM_rule(1, key, enable, process_data_command_r));


        enable.data[2] = 0;
        key.shorts[3] = 1;
        key.bytes[5] = 'c';
        key.bytes[4] = 'i';
        while (!TCAM::TCAM_rule(2, key, enable, process_control_command_i));


        // TODO: Update
        enable.shorts[3] = 0;
        key.bytes[5] = 'q';
        key.bytes[4] = 't';
        while (!TCAM::TCAM_rule(3, key, enable, process_query_request_t));
    }
    
    STATUS __not_in_flash_func(data_node)() {
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
                get_data(data.bytes, 12, true);
                pif (index == 12) {
                    TCAM::TCAM_process(&data);
                }               
                break;

            // Host protocol should create bubble waiting for status after sending data.
            //  Half duplex like currently for simplicity. We should have the bandwidth.
            //  Host needs to be on the ball though. Performance loss is possible from OS!
            case DATA_STATES::PAYLOAD:                              // Host should see ACTIVE_0 to ACTIVE_1
                process_payload();
                break;

            // Host protocol should create bubble waiting for status after sending data.
            //  Half duplex like currently for simplicity. We should have the bandwidth.
            //  Host needs to be on the ball though. Performance loss is possible from OS!
            case DATA_STATES::CHECKSUM_DELIMITER_PROCESS:           // Host should see ACTIVE_1 to IDLE_1/0 or READY
                get_data(data.bytes, 8, false);
                process_frame();
                break;

            // Host protocol should create bubble waiting for status after sending data.
            //  Half duplex like currently for simplicity. We should have the bandwidth.
            //  Host needs to be on the ball though. Performance loss is possible from OS!
            case DATA_STATES::READY:                                // Host should see READY to IDLE_1/0
                if (trigger) {
                    Serial::Protocol::internal::process(buf, len);
                    idle_num = (idle_num + 1) % 2;
                    state_data = DATA_STATES::SETUP;
                }
                break;
                
            // Host protocol should create bubble waiting for status containing response data.
            //  Half duplex like currently for simplicity.
            case DATA_STATES::READY_RESPONSE:
                if (acknowledge) {
                    idle_num = (idle_num + 1) % 2;
                    state_data = DATA_STATES::SETUP;
                }
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

    void __not_in_flash_func(trigger_processing)() {
        trigger = true;
    }

    void __not_in_flash_func(acknowledge_query)() {
        acknowledge = true;
    }

    void __not_in_flash_func(reset)() {
        state_data = DATA_STATES::SETUP;
    }

    void __not_in_flash_func(get_data)(uint8_t *buf, uint16_t len, bool checksum) {
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

    inline void __not_in_flash_func(process_payload)() {
        switch (command) {
            case COMMAND::DATA:
                get_data(buf->raw, len, true);

                if (len == index) {
                    state_data = DATA_STATES::CHECKSUM_DELIMITER_PROCESS;
                    time = time_us_64();
                    index = 0;
                    status = STATUS::ACTIVE_1;
                    trigger = false;
                }
                break;

            case COMMAND::RAW_DATA:
                get_data(buf->raw, len, false);

                if (len == index) {
                    state_data = DATA_STATES::CHECKSUM_DELIMITER_PROCESS;
                    time = time_us_64();
                    index = 0;
                    status = STATUS::ACTIVE_1;
                    trigger = false;
                }
                break;

            case COMMAND::SET_ID:
                get_data(data.bytes, 1, true);

                if (index == 1) {
                    state_data = DATA_STATES::CHECKSUM_DELIMITER_PROCESS;
                    time = time_us_64();
                    index = 0;
                    status = STATUS::ACTIVE_1;
                }
                break;

            case COMMAND::QUERY_TEST:
                // TODO: Get data?

                if (index == 0) {
                    state_data = DATA_STATES::CHECKSUM_DELIMITER_PROCESS;
                    time = time_us_64();
                    index = 0;
                    status = STATUS::ACTIVE_1;
                }
                break;

            default:
                state_data = DATA_STATES::SETUP;
                break;
        }
    }

    inline void __not_in_flash_func(process_frame)() {
        if (index == 8) {
            index = 0;
                    
            if (ntohl(data.data[1]) == 0xAEAEAEAE) {
                switch (command) {
                    case COMMAND::DATA:
                        // Future: Look into parity
                        if (ntohl(data.data[0]) == ~checksum) {
                            state_data = DATA_STATES::READY;
                            time = time_us_64();
                            status = STATUS::READY;
                            trigger = false;
                            return;
                        }
                        break;
                    
                    case COMMAND::RAW_DATA:
                        state_data = DATA_STATES::READY;
                        time = time_us_64();
                        status = STATUS::READY;
                        trigger = false;
                        return;

                    case COMMAND::SET_ID:
                        // Future: Look into parity
                        if (ntohl(data.data[0]) == ~checksum) {
                            Serial::Protocol::CONTROL_NODE::set_id(data.bytes[0]);
                            idle_num = (idle_num + 1) % 2;
                            state_data = DATA_STATES::SETUP;
                        }
                        break;

                    case COMMAND::QUERY_TEST:
                        if (ntohl(data.data[0]) == ~checksum) {
                            // TODO: Fill in the response packet
                            state_data = DATA_STATES::READY_RESPONSE;
                            status = STATUS::READY;
                            time = time_us_64();
                            acknowledge = false;
                            return;
                        }
                        break;

                    default:
                        break;
                }
            }
        }
    }

    void process_data_command_d() {
        state_data = DATA_STATES::PAYLOAD;
        time = time_us_64();
        command = COMMAND::DATA;
        status = STATUS::ACTIVE_0;
        index = 0;
        trigger = false;
    }

    void process_data_command_r() {
        state_data = DATA_STATES::PAYLOAD;
        time = time_us_64();
        command = COMMAND::RAW_DATA;
        status = STATUS::ACTIVE_0;
        index = 0;
        trigger = false;
    }

    void process_control_command_i() {
        state_data = DATA_STATES::PAYLOAD;
        time = time_us_64();
        command = COMMAND::SET_ID;
        status = STATUS::ACTIVE_0;
        index = 0;
    }

    void process_query_request_t() {
        index = 0;
        state_data = DATA_STATES::PAYLOAD;
        time = time_us_64();
        command = COMMAND::QUERY_TEST;
        status = STATUS::ACTIVE_0;
    }
}
/* 
 * File:   data_node.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "hardware/uart.h"
#include "Serial/serial_uart/serial_uart.h"
#include "Serial/serial_uart/internal.h"
#include "Serial/serial_uart/CRC.h"
#include "Serial/serial_uart/control_node.h"
#include "Serial/serial_uart/data_node.h"
#include "Serial/serial_uart/machine.h"
using Serial::UART::internal::STATUS;

namespace Serial::UART::DATA_NODE {
    static uint8_t *buf = 0;
    static uint16_t len = 0;

    static DATA_STATES state_data = DATA_STATES::SETUP;
    static uint8_t idle_num = 0;
    static COMMAND command;
    static uint32_t index;
    static random_type data;
    static uint32_t checksum;
    static STATUS status;
    static bool trigger;
    static uint64_t time;

    static void get_data(uint8_t *buf, uint16_t len, bool checksum);
    static void process_command();
    static void process_payload();
    static void process_frame();
    
    STATUS __not_in_flash_func(data_node)() {
        // Currently we drop the frame and wait for the next valid header.
        //  Host app will do the right thing using status messages.
        switch (state_data) {
            case DATA_STATES::SETUP:
                index = 0;
                trigger = false;
                checksum = 0xFFFFFFFF;
                Serial::UART::uart_callback(&buf, &len);
                state_data = DATA_STATES::PREAMBLE_CMD_LEN_T_ROWS_COLUMNS;
                time = time_us_64();

                if (idle_num == 0)
                    status = STATUS::IDLE_0;
                else
                    status = STATUS::IDLE_1;
                break;

            // Host protocol should create bubble waiting for status after sending data.
            //  Half duplex like currently for simplicity. We should have the bandwidth.
            //  Host needs to be on the ball though. Performance loss is possible from OS!
            case DATA_STATES::PREAMBLE_CMD_LEN_T_ROWS_COLUMNS:  // Host should see IDLE_0/1 to ACTIVE_0
                get_data(data.bytes, 12, true);
                process_command();                
                break;

            // Host protocol should create bubble waiting for status after sending data.
            //  Half duplex like currently for simplicity. We should have the bandwidth.
            //  Host needs to be on the ball though. Performance loss is possible from OS!
            case DATA_STATES::PAYLOAD:                          // Host should see ACTIVE_0 to ACTIVE_1
                process_payload();
                break;

            // Host protocol should create bubble waiting for status after sending data.
            //  Half duplex like currently for simplicity. We should have the bandwidth.
            //  Host needs to be on the ball though. Performance loss is possible from OS!
            case DATA_STATES::CHECKSUM_DELIMITER_PROCESS:       // Host should see ACTIVE_1 to IDLE_1/0 or READY
                get_data(data.bytes, 8, false);
                process_frame();
                break;


            // Host protocol should create bubble waiting for status after sending data.
            //  Half duplex like currently for simplicity. We should have the bandwidth.
            //  Host needs to be on the ball though. Performance loss is possible from OS!
            case DATA_STATES::READY:                            // Host should see READY to IDLE_1/0
                if (trigger) {
                    Serial::UART::internal::process((uint16_t *) buf, len);
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

    void __not_in_flash_func(reset)() {
        state_data = DATA_STATES::SETUP;
    }

    void __not_in_flash_func(get_data)(uint8_t *buf, uint16_t len, bool checksum) {
        while (index < len) {
            if (uart_is_readable(uart0)) {
                buf[index] = uart_getc(uart0);

                if (checksum)
                    checksum = Serial::UART::CRC::crc32(checksum, buf[index]);

                index++;
            }
            else
                break;
        }
    }

    inline void __not_in_flash_func(process_command)() {
        if (index == 8) {
            if (ntohl(data.longs[0]) == 0xAAEEAAEE) {
                switch (data.bytes[5]) {
                    case 'd':
                        switch (data.bytes[4]) {
                            case 'd':
                                // TODO: Verify DEFINE_SERIAL_RGB_TYPE use data.bytes[11]
                                if (ntohs(data.shorts[3]) == len &&
                                    data.bytes[8] == sizeof(DEFINE_SERIAL_RGB_TYPE) &&
                                    data.bytes[9] == Matrix::MULTIPLEX &&
                                    data.bytes[10] == Matrix::COLUMNS &&
                                    data.bytes[11] == 0
                                ) {
                                    state_data = DATA_STATES::PAYLOAD;
                                    time = time_us_64();
                                    command = COMMAND::DATA;
                                    status = STATUS::ACTIVE_0;
                                    index = 0;
                                    trigger = false;
                                }
                                break;

                            case 'r':
                                // TODO: Verify DEFINE_SERIAL_RGB_TYPE use data.bytes[11]
                                if (ntohs(data.shorts[3]) == len &&
                                    data.bytes[8] == sizeof(DEFINE_SERIAL_RGB_TYPE) &&
                                    data.bytes[9] == Matrix::MULTIPLEX &&
                                    data.bytes[10] == Matrix::COLUMNS &&
                                    data.bytes[11] == 0
                                ) {
                                    state_data = DATA_STATES::PAYLOAD;
                                    time = time_us_64();
                                    command = COMMAND::RAW_DATA;
                                    status = STATUS::ACTIVE_0;
                                    index = 0;
                                    trigger = false;
                                }
                                break;

                            default:
                                break;
                        }
                        break;

                    case 'c':
                        switch (data.bytes[4]) {
                            case 'i':
                                if (ntohs(data.shorts[3]) == 1 &&
                                    ntohl(data.longs[2]) == 0
                                ) {
                                    state_data = DATA_STATES::PAYLOAD;
                                    time = time_us_64();
                                    command = COMMAND::SET_ID;
                                    status = STATUS::ACTIVE_0;
                                    index = 0;
                                }
                                break;

                            default:
                                break;
                        }
                        break;

                    default:
                        break;
                }
            }
        }

        // Host app will do the right thing. (Did not see IDLE_0/1 to ACTIVE_0)
        state_data = DATA_STATES::SETUP;
    }

    inline void __not_in_flash_func(process_payload)() {
        switch (command) {
            case COMMAND::DATA:
                get_data(buf, len, true);

                if (len == index) {
                    state_data = DATA_STATES::CHECKSUM_DELIMITER_PROCESS;
                    time = time_us_64();
                    index = 0;
                    status = STATUS::ACTIVE_1;
                    trigger = false;
                }
                break;

            case COMMAND::RAW_DATA:
                get_data(buf, len, false);

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

            default:
                state_data = DATA_STATES::SETUP;
                break;
        }
    }

    inline void __not_in_flash_func(process_frame)() {
        bool error = true;

        if (index == 8) {
            index = 0;
                    
            if (ntohl(data.longs[1]) == 0xAEAEAEAE) {
                switch (command) {
                    case COMMAND::DATA:
                        // Future: Look into parity
                        if (ntohl(data.longs[0]) == ~checksum) {
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
                        if (ntohl(data.longs[0]) == ~checksum) {
                            Serial::UART::CONTROL_NODE::set_id(data.bytes[0]);
                            error = false;
                        }
                        break;

                    default:
                        break;
                }
            }
            
            if (!error) 
                idle_num = (idle_num + 1) % 2;

            state_data = DATA_STATES::SETUP;
        }
    }
}
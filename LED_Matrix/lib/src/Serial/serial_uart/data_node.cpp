/* 
 * File:   data_node.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "hardware/uart.h"
#include "Serial/serial_uart/serial_uart.h"
#include "Serial/serial_uart/internal.h"
#include "Serial/serial_uart/control_node.h"
#include "Serial/serial_uart/data_node.h"

namespace Serial::UART::DATA_NODE {
    static uint8_t *buf = 0;
    static uint16_t len = 0;
    
    // TODO: Refactor
    STATUS __not_in_flash_func(data_node)() {
        static DATA_STATES state_data = DATA_STATES::SETUP;
        static COMMAND command;
        static uint32_t index;
        static random_type data;
        static uint32_t checksum;
        static STATUS status;
        bool escape;


        // Currently we drop the frame and wait for the next valid header.
        //  Host app will do the right thing using status messages.
        switch (state_data) {
            case DATA_STATES::SETUP:
                index = 0;
                status = STATUS::IDLE;
                Serial::UART::uart_callback(&buf, &len);
                state_data = DATA_STATES::PREAMBLE_CMD_LEN;
                break;

            case DATA_STATES::PREAMBLE_CMD_LEN:             // Host protocol should create bubble waiting for status after sending data.
                escape = false;

                while (index < 8) {
                    if (uart_is_readable(uart0)) {
                        data.bytes[index] = uart_getc(uart0);
                        index++;
                    }
                    else
                        break;
                }

                if (index == 8) {
                    if (ntohl(data.longs[0]) == 0xAAEEAAEE) {
                        switch (data.bytes[5]) {
                            case 'd':
                                switch (data.bytes[4]) {
                                    case 'd':
                                        if (ntohs(data.shorts[3]) == len) {
                                            state_data = DATA_STATES::PAYLOAD;
                                            command = COMMAND::DATA;
                                            status = STATUS::ACTIVE_0;
                                            index = 0;
                                            checksum = 0;
                                        }
                                        else {
                                            escape = true;
                                        }
                                        break;

                                    default:
                                        escape = true;
                                        break;
                                }
                                break;

                            case 'c':
                                switch (data.bytes[4]) {
                                    case 'i':
                                        if (ntohs(data.shorts[3]) == 1) {
                                            state_data = DATA_STATES::PAYLOAD;
                                            command = COMMAND::SET_ID;
                                            status = STATUS::ACTIVE_0;
                                            index = 0;
                                            checksum = 0;
                                        }
                                        else {
                                            escape = true;
                                        }

                                    default:
                                        escape = true;
                                        break;
                                }
                                break;

                            default:
                                escape = true;
                                break;
                        }
                    }
                    else {
                        escape = true;
                    }
                }

                // Reseed and try again.
                //  Host app will do the right thing. (Did not see 'r' to 'a')
                if (escape) {
                    data.bytes[0] = data.bytes[1];
                    data.bytes[1] = data.bytes[2];
                    data.bytes[2] = data.bytes[3];
                    data.bytes[3] = data.bytes[4];
                    data.bytes[4] = data.bytes[5];
                    data.bytes[5] = data.bytes[6];
                    data.bytes[6] = data.bytes[7];
                    index--;
                }
                break;

            case DATA_STATES::PAYLOAD:                      // Host protocol should create bubble waiting for status after sending data.
                switch (command) {
                    case COMMAND::DATA:
                        while (index < len) {
                            if (uart_is_readable(uart0)) {
                                buf[index] = uart_getc(uart0);
                                index++;
                            }
                            else
                                break;
                        }

                        // TODO: Compute checksum in parallel (via DMA?)

                        if (len == index) {
                            state_data = DATA_STATES::CHECKSUM_DELIMITER_PROCESS;
                            index = 0;
                            status = STATUS::ACTIVE_1;
                        }
                        break;
                    case COMMAND::SET_ID:
                        while (index < 1) {
                            if (uart_is_readable(uart0)) {
                                data.bytes[index] = uart_getc(uart0);
                                index++;
                            }
                            else
                                break;
                        }

                        // TODO: Compute checksum in parallel (via DMA?)

                        if (index == 1) {
                            Serial::UART::CONTROL_NODE::set_id(data.bytes[0]);
                            state_data = DATA_STATES::CHECKSUM_DELIMITER_PROCESS;
                            index = 0;
                            status = STATUS::ACTIVE_1;
                        }
                        break;

                    default:
                        state_data = DATA_STATES::PREAMBLE_CMD_LEN;
                        status = STATUS::IDLE;
                        break;
                }
                break;

            case DATA_STATES::CHECKSUM_DELIMITER_PROCESS:   // Host protocol should create bubble waiting for status after sending data.
                while (index < 8) {
                    if (uart_is_readable(uart0)) {
                        data.bytes[index] = uart_getc(uart0);
                        index++;
                    }
                    else
                        break;
                }

                if (index == 8) {
                    index = 0;
                    
                    // We are allowed to block here
                    if (ntohl(data.longs[0]) == checksum && ntohl(data.longs[1]) == 0xAEAEAEAE) {
                        switch (command) {
                            case COMMAND::DATA:
                                Serial::UART::internal::process((uint16_t *) buf, len);
                                break;
                            default:
                                break;
                        }

                        state_data = DATA_STATES::SETUP;
                    }
                    // TODO: At this point we have lost the frame (reliable delivery option not possible currently)
                    else {
                        state_data = DATA_STATES::PREAMBLE_CMD_LEN;
                        status = STATUS::IDLE;
                    }
                }
                break;

            default:
                state_data = DATA_STATES::SETUP;
                break;
        }

        return status;
    }
}
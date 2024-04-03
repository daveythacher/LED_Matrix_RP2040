/*
 * File:   serial_uart.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include <stdint.h>
#include <machine/endian.h>
#include "hardware/gpio.h"
#include "hardware/uart.h"
#include "Serial/serial_uart/serial_uart.h"
#include "Matrix/matrix.h"

namespace Serial {
    static uint8_t *buf = 0;
    static uint16_t len = 0;

    #if __BYTE_ORDER == __LITTLE_ENDIAN
        #define ntohs(x) __bswap16(x)
        #define ntohl(x) __bswap32(x)
        #define htons(x) __bswap16(x)
        #define htonl(x) __bswap32(x)
    #else
        #define ntohs(x) ((uint16_t)(x))
        #define ntoh1(x) ((uint32_t)(x))
        #define htons(x) ((uint16_t)(x))
        #define hton1(x) ((uint32_t)(x))
    #endif

    enum class DATA_STATES {
        SETUP,
        PREAMBLE_CMD_LEN,
        PAYLOAD,    // TODO: Compute checksum in parallel (via DMA?)
        CHECKSUM_DELIMITER_PROCESS
    };

    enum class CONTROL_STATES {
        SETUP,
        LISTEN,
        TRIGGER,
        RESET       // TODO: Make non-blocking
    };

    union random_type {
        uint8_t bytes[8];
        uint16_t shorts[4];
        uint32_t longs[2];
    };

    enum class STATUS {
        IDLE,
        ACTIVE_0,
        ACTIVE_1
    };

    enum class COMMAND {
        DATA
    };

    struct Status_Message {
        Status_Message() {
            header = htonl(0xAAEEAAEE);
            cmd[0] = 's';
            cmd[1] = 's';
            len = htons(4);
            delimiter = htonl(0xAEAEAEAE);
        }

        void set_status(STATUS s) {
            switch (s) {
                case STATUS::IDLE:
                    status = htonl(0);
                    // TODO: Checksum
                    checksum = htonl(0);
                    break;
                case STATUS::ACTIVE_0:
                    status = htonl(1);
                    // TODO: Checksum
                    checksum = htonl(0);
                    break;
                case STATUS::ACTIVE_1:
                    status = htonl(2);
                    // TODO: Checksum
                    checksum = htonl(0);
                    break;
                default:
                    status = htonl(0xFFFFFFFF);
                    // TODO: Checksum
                    checksum = htonl(0);
                    break;
            }
        }

        uint32_t header;
        uint8_t cmd[2];
        uint16_t len;
        uint32_t status;
        uint32_t checksum;
        uint32_t delimiter;
    };

    static void uart_process();
    static void send_status(STATUS status);
    static uint8_t send_message(Status_Message *message, bool block);

    void uart_start() {
        // IO
        gpio_init(0);
        gpio_init(1);
        gpio_init(5);
        gpio_set_dir(0, GPIO_OUT);
        gpio_set_function(0, GPIO_FUNC_UART);
        gpio_set_function(1, GPIO_FUNC_UART);
        gpio_set_function(5, GPIO_FUNC_UART);

        // UART
        static_assert(SERIAL_UART_BAUD <= 7800000, "Baud rate must be less than 7.8MBaud");
        uart_init(uart0, SERIAL_UART_BAUD);
        uart_init(uart1, 115200);
    }

    // Warning host is required to obey flow control and handle bus recovery
    void __not_in_flash_func(uart_task)() {
        static DATA_STATES state_data = DATA_STATES::SETUP;
        static CONTROL_STATES state_control = CONTROL_STATES::SETUP;
        static COMMAND command;
        static uint32_t index;
        static random_type data;
        static STATUS status;
        static uint32_t checksum;
        bool escape;

        // Check for errors
        if (!((uart0_hw->ris & 0x380) == 0)) {
            uart0_hw->icr = 0x7FF;
        }

        // Check for errors
        if (!((uart1_hw->ris & 0x380) == 0)) {
            uart1_hw->icr = 0x7FF;
        }

        // TODO:
        // Reply via uart0 using correct control header (if required)
        switch (state_control) {
            case CONTROL_STATES::SETUP:
                // TODO:
                state_control = CONTROL_STATES::LISTEN;
                break;
            case CONTROL_STATES::LISTEN:
                // TODO:
                //state_control = CONTROL_STATES::TRIGGER;
                break;
            case CONTROL_STATES::TRIGGER:
                // TODO:
                state_control = CONTROL_STATES::SETUP;
                break;
            case CONTROL_STATES::RESET:
                // TODO:
                state_control = CONTROL_STATES::SETUP;
                break;
            default:
                state_control = CONTROL_STATES::SETUP;
                break;
        }

        // Currently we drop the frame and wait for the next valid header.
        //  Host app will do the right thing using status messages.
        //
        // Currently we do not implement a valid control header for status messages.
        switch (state_data) {
            case DATA_STATES::SETUP:
                index = 0;
                status = STATUS::IDLE;
                uart_callback(&buf, &len);
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
                        // Currently we only support the one command
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
                while (index < len) {
                    if (uart_is_readable(uart0)) {
                        buf[index] = uart_getc(uart0);
                        index++;
                    }
                    else
                        break;
                }

                // TODO: Compute checksum

                if (len == index) {
                    state_data = DATA_STATES::CHECKSUM_DELIMITER_PROCESS;
                    index = 0;
                    status = STATUS::ACTIVE_1;
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
                                uart_process();
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

        send_status(status);
    }

    void __not_in_flash_func(uart_process)() {
        uint16_t *p = (uint16_t *) buf;

        switch (sizeof(DEFINE_SERIAL_RGB_TYPE)) {
            case 2:
            case 6:
                for (uint16_t i = 0; i < len; i += 2)
                    p[i / 2] = ntohs(p[i / 2]);
                break;
            default:
                break;
        }

        Matrix::Worker::process((void *) buf);
    }

    void __not_in_flash_func(send_status)(STATUS status) {
        static Status_Message messages[6];
        static uint8_t front_counter = 0;
        static uint8_t back_counter = 0;

        if (back_counter < 6) {
            // TODO:
        }
        else {
            // TODO:
        }

        messages[front_counter].set_status(status);
        back_counter += send_message(&messages[front_counter], false);
        front_counter = (front_counter + 1) / 6;
        back_counter %= 6;
    }

    uint8_t __not_in_flash_func(send_message)(Status_Message *buf, bool block) {
        // TODO: Use uart1
        return 0;
    }
}

#include "pico/multicore.h"
#include "hardware/uart.h"
#include "Serial/serial_uart/internal.h"
#include "Serial/serial_uart/serial_uart.h"
#include "Matrix/matrix.h"

namespace Serial::UART::internal {
    void __not_in_flash_func(process)(uint16_t *p, uint16_t len) {
        switch (sizeof(DEFINE_SERIAL_RGB_TYPE)) {
            case 2:
            case 6:
                for (uint16_t i = 0; i < len; i += 2)
                    p[i / 2] = ntohs(p[i / 2]);
                break;
            default:
                break;
        }

        Matrix::Worker::process((void *) p);
    }

    void __not_in_flash_func(send_status)(STATUS status) {
        static Status_Message messages;

        messages.set_status(status);
        send_message(&messages, true);
    }

    void __not_in_flash_func(send_message)(Status_Message *buf, bool block) {
        // TODO: Use uart0
        // TODO: Make sure watchdog does not fire in here
    }
}
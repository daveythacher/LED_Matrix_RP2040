#include "pico/multicore.h"
#include "hardware/uart.h"
#include "Serial/serial_uart/internal.h"

namespace Serial::internal {
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
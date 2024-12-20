/* 
 * File:   internal.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "hardware/watchdog.h"
#include "Serial/Protocol/Serial/internal.h"
#include "Serial/Node/data.h"
#include "System/machine.h"
#include "Matrix/matrix.h"

namespace Serial::Protocol::internal {
    void __not_in_flash_func(process)(Serial::packet *p, uint16_t len, bool isBuffer) {
        switch (sizeof(DEFINE_SERIAL_RGB_TYPE)) {
            case 2:
            case 6:
                for (uint16_t i = 0; i < len; i += 2)
                    p->val[i / 2] = ntohs(p->val[i / 2]);
                break;
            default:
                break;
        }

        Matrix::Worker::process(p, isBuffer);
    }

    void __not_in_flash_func(send_status)(STATUS status) {
        static Status_Message messages;

        static_assert(sizeof(Status_Message) <= 32, "Status message too big for UART FIFO on RP2040");

        messages.set_status(status);
        send_message(&messages);
    }

    static inline void __not_in_flash_func(write_chunk)(uint32_t v, uint8_t bits) {
        for (int i = 0; i < bits; i += 8) {
            if (Serial::Node::Data::isAvailable())
                Serial::Node::Data::putc((v >> i) & 0xFF);
            else
                watchdog_update();
        }
    }

    // Blocking buts keeps watchdog going
    void __not_in_flash_func(send_message)(Status_Message *buf) {
        write_chunk(buf->header, 32);
        write_chunk(buf->cmd, 8);
        write_chunk(buf->len, 16);
        write_chunk(buf->status, 32);
        write_chunk(buf->checksum, 32);
        write_chunk(buf->delimiter, 32);
    }
}
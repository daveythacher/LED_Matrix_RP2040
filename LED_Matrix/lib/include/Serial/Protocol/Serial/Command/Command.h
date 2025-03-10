/* 
 * File:   Comand.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef SERIAL_PROTOCOL_SERIAL_COMMAND_H
#define SERIAL_PROTOCOL_SERIAL_COMMAND_H

#include "Serial/Protocol/Serial/internal.h"
#include "Serial/config.h"
#include "TCAM/TCAM.h"

namespace Serial::Protocol::DATA_NODE {
    // TODO: Clean up this mess
    class Command : public TCAM::Handler {
        public:
            static Serial::Protocol::internal::STATUS data_node();
            static void trigger_processing();
            static void acknowledge_query();
            static void reset();

            virtual void callback();

        protected:
            static bool get_data(uint8_t *buf, uint16_t len, bool checksum);
            static void error();
            static void clear_trigger();
            static void update_time();

            virtual void process_frame_internal() = 0;
            virtual void process_command_internal() = 0;
            virtual void process_payload_internal() = 0;
            virtual void process_internal(uint8_t *buf, uint16_t len) = 0;

            enum class DATA_STATES {
                SETUP,
                PREAMBLE_CMD_LEN_T_MULTIPLEX_COLUMNS,
                PAYLOAD,
                CHECKSUM_DELIMITER_PROCESS,
                READY,
                READY_RESPONSE,
                ERROR
            };

            struct uint128_t {
                uint8_t b[128 / 8];
                uint16_t s[128 / 16];
                uint32_t l[128 / 32];
                uint64_t ll[128 / 64];
            };

            // TODO: Sort this out
            static uint8_t *buf;
            static uint16_t len;
            static DATA_STATES state_data;
            static uint8_t idle_num;
            static Serial::Protocol::internal::STATUS status;
            static uint32_t checksum;
            static uint128_t data;

        private:
            static uint64_t time;
            static uint32_t index;
            static bool trigger;
            static bool acknowledge;
            static Command *ptr;
    };
}

#endif

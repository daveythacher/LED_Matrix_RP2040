/* 
 * File:   Comand.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef SERIAL_PROTOCOL_SERIAL_COMMAND_H
#define SERIAL_PROTOCOL_SERIAL_COMMAND_H

#include "Serial/Protocol/Serial/internal.h"
#include "Serial/config.h"
#include "TCAM/tcam.h"

namespace Serial::Protocol::DATA_NODE {
    class Command : public TCAM::Handler {
        public:
            static Serial::Protocol::internal::STATUS data_node();
            static void trigger_processing();
            static void acknowledge_query();
            static void reset();

            virtual void callback();

        protected:
            static void get_data(uint8_t *buf, uint16_t len, bool checksum);
            static void error();

            virtual void process_frame_internal() = 0;
            virtual void process_command_internal() = 0;
            virtual void process_payload_internal() = 0;
            virtual void process_internal(Serial::packet *buf, uint16_t len) = 0;

            enum class DATA_STATES {
                SETUP,
                PREAMBLE_CMD_LEN_T_MULTIPLEX_COLUMNS,
                PAYLOAD,
                CHECKSUM_DELIMITER_PROCESS,
                READY,
                READY_RESPONSE,
                ERROR
            };

            static Serial::packet *buf;
            static uint16_t len;
            static DATA_STATES state_data;
            static uint8_t idle_num;
            static uint32_t index;
            static SIMD::SIMD_SINGLE<uint32_t> data;
            static uint32_t checksum;
            static Serial::Protocol::internal::STATUS status;
            static bool trigger;
            static bool acknowledge;
            static uint64_t time;
            static Command *ptr;

            friend class filter;
    };
}

#endif

/* 
 * File:   filter.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef SERIAL_PROTOCOL_SERIAL_COMMAND_FILTER_H
#define SERIAL_PROTOCOL_SERIAL_COMMAND_FILTER_H

namespace Serial::Protocol::DATA_NODE {
    class filter {
        public:
            static void filter_setup();

        private:
            static void process_data_command_d();
            static void process_data_command_r();
            static void process_control_command_i();
            static void process_query_request_t();
    };
}

#endif

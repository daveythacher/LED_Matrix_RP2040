/* 
 * File:   filter.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "Serial/Protocol/Serial/Command/Command.h"
#include "Serial/Node/data.h"
#include "System/machine.h"
#include "TCAM/tcam.h"
using Serial::Protocol::internal::STATUS;

namespace Serial::Protocol::DATA_NODE {
    static void process_data_command_d();
    static void process_data_command_r();
    static void process_control_command_i();
    static void process_query_request_t();

    void filter_setup() {
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

    void process_data_command_d() {
        /*state_data = DATA_STATES::PAYLOAD;
        time = time_us_64();
        command = COMMAND::DATA;
        status = STATUS::ACTIVE_0;
        index = 0;
        trigger = false;*/
        
        Command::process_command();
    }

    void process_data_command_r() {
        /*state_data = DATA_STATES::PAYLOAD;
        time = time_us_64();
        command = COMMAND::RAW_DATA;
        status = STATUS::ACTIVE_0;
        index = 0;
        trigger = false;*/
        
        Command::process_command();
    }

    void process_control_command_i() {
        /*state_data = DATA_STATES::PAYLOAD;
        time = time_us_64();
        command = COMMAND::SET_ID;
        status = STATUS::ACTIVE_0;
        index = 0;*/
        
        Command::process_command();
    }

    void process_query_request_t() {
        /*index = 0;
        state_data = DATA_STATES::PAYLOAD;
        time = time_us_64();
        command = COMMAND::QUERY_TEST;
        status = STATUS::ACTIVE_0;*/

        Command::process_command();
    }
}
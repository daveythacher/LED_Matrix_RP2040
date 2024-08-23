/* 
 * File:   filter.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "Serial/Protocol/Serial/Command/Data/Data/Data.h"
#include "Serial/Protocol/Serial/Command/Data/Raw_Data/Raw_Data.h"
#include "Serial/Protocol/Serial/Command/Data/ID/ID.h"
#include "Serial/Protocol/Serial/Command/Query/Test/Test.h"
#include "Serial/Node/data.h"
#include "System/machine.h"
#include "TCAM/tcam.h"

namespace Serial::Protocol::DATA_NODE {
    void filter::filter_setup() {
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

    void __not_in_flash_func(filter::process_data_command_d)() {
        static Data data;
        Command::ptr = &data;        
        Command::process_command();
    }

    void __not_in_flash_func(filter::process_data_command_r)() {
        static Raw_Data data;
        Command::ptr = &data;
        Command::process_command();
    }

    void __not_in_flash_func(filter::process_control_command_i)() {
        static ID id;
        Command::ptr = &id;        
        Command::process_command();
    }

    void __not_in_flash_func(filter::process_query_request_t)() {
        static Test test;
        Command::ptr = &test;
        Command::process_command();
    }
}
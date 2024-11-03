/* 
 * File:   filter.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "Serial/Protocol/Serial/filter.h"
#include "Serial/Protocol/Serial/Command/Data/Data/Data.h"
#include "Serial/Protocol/Serial/Command/Data/Raw_Data/Raw_Data.h"
#include "Serial/Protocol/Serial/Command/Data/ID/ID.h"
#include "Serial/Protocol/Serial/Command/Query/Test/Test.h"
#include "Serial/Node/data.h"
#include "System/machine.h"
#include "TCAM/tcam.h"

namespace Serial::Protocol::DATA_NODE {
    TCAM::Table<SIMD::SIMD_SINGLE<uint32_t>> data_filter;

    void filter::filter_setup() {
        static Data data;
        static Raw_Data raw;
        static Test test;
        static ID id;

        SIMD::SIMD_SINGLE<uint32_t> key;
        SIMD::SIMD_SINGLE<uint32_t> enable;

        // TCAM can covert 6-12 operations down to 3.
        //  The conditionals can be removed with AND down to 1.
        enable.l[0] = 0xFFFFFFFF;
        enable.l[1] = 0xFFFFFFFF;
        enable.l[2] = 0xFFFFFFFF;

        key.l[0] = htonl(0xAAEEAAEE);
        key.b[4] = 'd';
        key.b[5] = 'd';
        key.s[3] = htons(Serial::Node::Data::get_len());
        key.b[8] = sizeof(DEFINE_SERIAL_RGB_TYPE);
        key.b[9] = Matrix::MULTIPLEX;
        key.b[10] = Matrix::COLUMNS;
        key.b[11] = DEFINE_SERIAL_RGB_TYPE::id;
        while (!data_filter.TCAM_rule(0, key, enable, &data));

        key.b[4] = 'r';
        while (!data_filter.TCAM_rule(1, key, enable, &raw));


        enable.l[2] = 0;
        key.s[3] = 1;
        key.b[5] = 'c';
        key.b[4] = 'i';
        while (!data_filter.TCAM_rule(2, key, enable, &id));


        // TODO: Update
        enable.s[3] = 0;
        key.b[5] = 'q';
        key.b[4] = 't';
        while (!data_filter.TCAM_rule(3, key, enable, &test));
    }
}
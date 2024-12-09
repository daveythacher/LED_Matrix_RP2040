/* 
 * File:   filter.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "Serial/Protocol/Serial/filter.h"
#include "Serial/Protocol/Serial/Command/Data/Data/Data.h"
#include "Serial/Protocol/Serial/Command/Data/Buffer/Buffer.h"
#include "Serial/Protocol/Serial/Command/Data/Raw_Data/Raw_Data.h"
#include "Serial/Protocol/Serial/Command/Data/ID/ID.h"
#include "Serial/Protocol/Serial/Command/Query/Test/Test.h"
#include "Serial/Node/data.h"
#include "System/machine.h"

namespace Serial::Protocol::DATA_NODE {
    TCAM::Table<SIMD::SIMD_SINGLE<uint8_t>> filter::data_filter;

    void filter::filter_setup() {
        Data *data = new Data();
        Raw_Data *raw = new Raw_Data();
        Test *test = new Test();
        ID *id = new ID();
        Buffer *buf = new Buffer();

        SIMD::SIMD_SINGLE<uint8_t> key;
        SIMD::SIMD_SINGLE<uint8_t> enable;

        for (uint8_t i = 0; i < SIMD::SIMD_SINGLE<uint8_t>::size(); i++) {
            enable.set(0xFF, i);
        }

        key.set(0xAA, 0);
        key.set(0xEE, 1);
        key.set(0xAA, 2);
        key.set(0xEE, 3);
        key.set('d', 4);
        key.set('d', 5);
        key.set(htons(Serial::Node::Data::get_len()) >> 8, 6);
        key.set(htons(Serial::Node::Data::get_len()) & 0xFF, 7);
        key.set(sizeof(DEFINE_SERIAL_RGB_TYPE), 8);
        key.set(Matrix::MULTIPLEX, 9);
        key.set(Matrix::COLUMNS, 10);
        key.set(DEFINE_SERIAL_RGB_TYPE::id, 11);
        while (!data_filter.TCAM_rule(0, key, enable, data));

        key.set('r', 5);
        while (!data_filter.TCAM_rule(1, key, enable, raw));

        key.set('b', 5);
        while (!data_filter.TCAM_rule(4, key, enable, buf));

        enable.set(0, 8);
        enable.set(0, 9);
        enable.set(0, 10);
        enable.set(0, 11);
        key.set(0, 6);
        key.set(1, 7);
        key.set('c', 4);
        key.set('i', 5);
        while (!data_filter.TCAM_rule(2, key, enable, id));

        // TODO: Update
        enable.set(0, 6);
        enable.set(0, 7);
        key.set('q', 4);
        key.set('t', 5);
        while (!data_filter.TCAM_rule(3, key, enable, test));

        // TODO: Create null filter?
    }
}
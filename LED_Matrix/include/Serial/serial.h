/* 
 * File:   serial.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef SERIAL_H
#define SERIAL_H

#include "Serial/config.h"

namespace Serial {
    /**
     *  @brief Starts Serial state machine
     *  @details Implemented in src/<app>/serial.cpp
     */
    void start();
    
    /**
     *  @brief Callback for Serial state machine
     *  @details Implemented in src/<app>/serial.cpp
     */
    void task();

    /**
     *  @brief Callback for data packet
     *  @details Implemented in src/<app>/serial.cpp
     */
    void callback(Serial::packet **buf);

    /**
     *  @brief Get size of data packet
     *  @details Implemented in src/<app>/serial.cpp
     */
    uint16_t get_len();
}

#endif


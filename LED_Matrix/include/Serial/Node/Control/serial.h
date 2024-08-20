/* 
 * File:   serial.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef SERIAL_NODE_CONTROL_SERIAL_H
#define SERIAL_NODE_CONTROL_SERIAL_H

namespace Serial::Node::Control {
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
}

#endif


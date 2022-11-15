/* 
 * File:   serial.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef SERIAL_H
#define SERIAL_H

    /**
     *  @brief Starts Serial state machine
     *  @details Implemented in src/<app>/serial.cpp
     */
    void serial_start();
    
    /**
     *  @brief Callback for Serial state machine
     *  @details Only used for testing
     *  @details Implemented in src/<app>/serial.cpp
     */
    void serial_task();
    
    /**
     *  @brief Serial state machine ISR from DMA/serial
     *  @details Implemented in Serial/<implementation>/serial_<implementation>.cpp
     */
    void serial_dma_isr();

#endif


/* 
 * File:   matrix.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef MATRIX_H
#define MATRIX_H

    /**
     *  @brief Starts Matrix state machine.
     *  @details Implemented in Matrix/<implementation>/matrix.cpp
     */
    void matrix_start();
    
    /**
     *  @brief Matrix state machine ISR from DMA/PIO.
     *  @details Mapped to ISR in src/<app>/isr.cpp
     *  @details Implemented in Matrix/<implementation>/matrix.cpp
     */
    void matrix_dma_isr();
    
    /**
     *  @brief Matrix ISR from Blank time.
     *  @details Mapped to ISR in src/<app>/isr.cpp
     *  @details Implemented in Matrix/<implementation>/matrix.cpp
     */
    void matrix_timer_isr();
    
    /**
     *  @brief State machine for converting RGB-24 to bitplanes.
     *  @details Usually runs on Core 1 processing buffers passed over FIFO from Core 0.
     *  @details Implemented in Matrix/<implementation>/worker.cpp
     */
    void work();
    
    /**
     *  @brief Function used to pass data to worker
     *  @details Implemented in Matrix/<implementation>/worker.cpp
     *  @return Return true if processing was not discarded
     */
    bool process(void *arg, bool block);

    /**
     *  @brief Variable used to capture timer/alarm index
     *  @details Implemented in Matrix/<implementation>/worker.cpp
     */
    extern volatile int timer;

#endif


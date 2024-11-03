/* 
 * File:   matrix.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef MATRIX_H
#define MATRIX_H

#include "Matrix/Buffer.h"

namespace Matrix {
    /**
     *  @brief Starts Matrix state machine.
     *  @details Implemented in Matrix/<implementation>/matrix.cpp
     */
    void start();
    
    /**
     *  @brief Matrix state machine ISR from DMA/PIO.
     *  @details Mapped to ISR in src/<app>/isr.cpp
     *  @details Implemented in Matrix/<implementation>/matrix.cpp
     */
    void dma_isr();
    
    /**
     *  @brief Matrix ISR from Blank time.
     *  @details Mapped to ISR in src/<app>/isr.cpp
     *  @details Implemented in Matrix/<implementation>/matrix.cpp
     *  @details This may be capable of being rendered into a coprocessor. (Currently just interrupts core 0.)
     */
    void timer_isr();

    /**
     *  @brief Variable used to capture timer/alarm index
     *  @details Implemented in Matrix/<implementation>/matrix.cpp
     */
    extern volatile int timer;
    
    // This can be rendered into coprocessor. (Currently this is done via super loop on core 1.)
    namespace Worker {
        /**
         *  @brief State machine for converting RGB-24 to bitplanes.
         *  @details Usually runs on Core 1 processing buffers passed over FIFO from Core 0.
         *  @details Implemented in Matrix/<implementation>/worker.cpp
         */
        void work();
        
        /**
         *  @brief Function used to pass data to worker (Assumes flow control)
         *  @details Implemented in Matrix/<implementation>/worker.cpp
         */
        void process(Serial::packet *buffer);

        /**
         *  @brief Function used to pass data thru worker (Assumes flow control)
         *  @details Implemented in Matrix/<implementation>/worker.cpp
         *  @details Buffer is copied into back buffer. (Do not use front or back buffer(s).)
         */
        void process(Matrix::Buffer *buffer);
    }
}

#endif


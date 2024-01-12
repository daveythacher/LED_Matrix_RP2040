/* 
 * File:   matrix.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef MATRIX_H
#define MATRIX_H

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
     */
    void timer_isr();

    /**
     *  @brief Variable used to capture timer/alarm index
     *  @details Implemented in Matrix/<implementation>/matrix.cpp
     */
    extern volatile int timer;
    
    namespace Worker {
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
    }

    // This is terrible practice!!!
    namespace Loafer {        
        /**
         *  @brief Function used to chuck data to loafer
         *  @details Implemented in Matrix/<implementation>/loafer.cpp
         *  @return Return true if processing was not discarded
         */
        bool toss(void *arg, bool block);

        /**
         * @brief Function used to grab a back buffer
         * @details Implemented in Matrix/<implementation>/loafer.cpp
         * @return Raw memory pointer to back buffer, no type is known by API. (Not very classy)
         */
        void *get_back_buffer();
    }

    namespace Calculator {
        /**
         * @brief Function used to verify configuration
         * @details Implemented in Matrix/<implementation>/matrix.cpp
         */
        void verify_configuration();
    }
}

#endif


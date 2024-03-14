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
        void process(void *arg);
    }

    // This is terrible practice!!!
    namespace Loafer {        
        /**
         *  @brief Function used to chuck data to loafer (Assumes flow control)
         *  @details Warning must toss buffer before asking for another.
         *  @details Implemented in Matrix/<implementation>/loafer.cpp
         */
        void toss(void *arg);

        /**
         *  @brief Function used to grab a back buffer
         *  @details Warning must only have one back buffer at a time.
         *  @details Implemented in Matrix/<implementation>/loafer.cpp
         *  @return Raw memory pointer to back buffer, no type is known by API. (Not very classy)
         */
        void *get_back_buffer();

        /**
         *  @brief Function to get size of back buffer
         *  @details Implemended in Matrix/<implementation/loafer.cpp
         *  @details Must always return a number which is a multiple of two!
         *  @return Buffer size
        */
        uint32_t get_buffer_size();
    }

    namespace Calculator {
        /**
         *  @brief Function used to verify configuration
         *  @details Implemented in Matrix/<implementation>/matrix.cpp
         */
        void verify_configuration();
    }
}

#endif


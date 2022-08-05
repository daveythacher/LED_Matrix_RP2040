/* 
 * File:   isr.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef ISR_H
#define ISR_H

    /**
     *  @brief Starts ISR handlers on Core 0
     *  @details Implemented in src/<app>/isr.cpp
     */
    void isr_start();
    
    /**
     *  @brief Starts ISR handlers on Core 1
     *  @details Implemented in src/<app>/isr.cpp
     */
    void isr_start_core1();

#endif


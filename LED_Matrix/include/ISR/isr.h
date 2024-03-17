/* 
 * File:   isr.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef ISR_H
#define ISR_H

namespace APP {

    /**
     *  @brief Starts ISR handlers on Core 0
     *  @details Implemented in src/<app>/isr.cpp
     */
    void isr_start();
}

#endif


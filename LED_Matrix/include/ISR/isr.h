/* 
 * File:   isr.h
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#ifndef ISR_H
#define ISR_H

namespace APP {

    /**
     *  @brief Starts ISR handlers on Core 1
     *  @details Implemented in src/<app>/isr.cpp
     *  @details Must always call after Matrix::start(), to initialize Matrix::timer variable.
     */
    void isr_start_core1();
}

#endif


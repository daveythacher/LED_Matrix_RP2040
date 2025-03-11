/* 
 * File:   Frame.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "Concurrent/Frame/Frame.h"

namespace Concurrent {
    template <typename T> Frame<T>::Frame() {

    }

    template <typename T> Frame<T>::~Frame() {

    }

    // This function cannot be reordered ever!
    //  Neither by CPU or by compiler
    //  Multithreading is handled by func implementation
    template <typename T> T *Frame<T>::run() {
        T *result;

        // TODO: Figure out compiler guard!

        {   // Produce result
            result = func();
        } 
        {   // Flush context including result
            sync_cpu();
            sync_cache();   // Blocking call
            sync_cpu();     // Blocking call
        }

        // Commit result
        return result;
    }

    template <typename T> void Frame<T>::sync_cpu() {

    }

    template <typename T> void Frame<T>::sync_cache() {

    }
}
/* 
 * File:   PWM_Matrix.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "Matrix/Mono/PWM/PWM_Matrix.h"
#include "Matrix/Mono/PWM/PWM_Packet.h"
#include "Matrix/Mono/hw_config.h"

namespace Matrix {
    PWM_Matrix *PWM_Matrix::ptr = nullptr;

    PWM_Matrix::PWM_Matrix() {
        _worker = PWM_Worker::get_worker(MULTIPLEX, STEPS, COLUMNS);
    }

    PWM_Matrix *PWM_Matrix::create_matrix() {
        if (ptr == nullptr) {
            ptr = new PWM_Matrix();
        }

        return ptr;
    }

    PWM_Matrix::~PWM_Matrix() {
        delete _worker;
    }
    
    void PWM_Matrix::show(unique_ptr<Packet> &packet) {
        unique_ptr<Packet> p(get_packet());
        packet.swap(p);
        _worker->convert(p.release());
    }
    
    unique_ptr<Packet> PWM_Matrix::get_packet() {
        unique_ptr<PWM_Packet> result(PWM_Packet::create_packet(MULTIPLEX, STEPS, COLUMNS));
        return result;
    }
}

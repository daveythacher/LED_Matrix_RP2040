/* 
 * File:   Matrix.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "Matrix/HUB75/PWM/PWM_Matrix.h"
#include "Matrix/HUB75/PWM/PWM_Packet.h"
#include "Matrix/HUB75/hw_config.h"

namespace Matrix {
    PWM_Matrix *PWM_Matrix::ptr = nullptr;

    PWM_Matrix::PWM_Matrix() {
        multiplex = new PWM_Multiplex();
    }

    PWM_Matrix *PWM_Matrix::create_matrix() {
        if (ptr == nullptr) {
            ptr = new PWM_Matrix();
        }

        return ptr;
    }
    
    void PWM_Matrix::show(unique_ptr<Packet> &packet) {
        unique_ptr<Packet> p(get_packet());
        packet.swap(p);
        multiplex->show(static_cast<PWM_Packet *>(p.release()));
    }
    
    unique_ptr<Packet> PWM_Matrix::get_packet() {
        unique_ptr<Packet> result(PWM_Packet::create_pwm_packet(MULTIPLEX, STEPS, COLUMNS));
        return result;
    }
}

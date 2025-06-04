/* 
 * File:   Matrix.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "Matrix/BUS8/PWM/Matrix.h"
#include "Matrix/BUS8/PWM/Packet.h"
#include "Matrix/BUS8/hw_config.h"

namespace Matrix::BUS8::PWM {
    Matrix *Matrix::ptr = nullptr;

    Matrix::Matrix() {
        multiplex = new Multiplex();
    }

    Matrix *Matrix::create_matrix() {
        if (ptr == nullptr) {
            ptr = new Matrix();
        }

        return ptr;
    }
    
    // No more double buffering
    void Matrix::show(::Matrix::Packet *packet) {
        if (packet != nullptr) {
            Packet *p = Packet::create_packet(packet, MULTIPLEX, STEPS, COLUMNS);

            if (p != nullptr) {
                multiplex->show(p);
            }
        }
    }
    
    ::Matrix::Packet *Matrix::get_packet() {
        return Packet::create_packet(MULTIPLEX, STEPS, COLUMNS);
    }

    void Matrix::work() {
        multiplex->work();
    }

    void Matrix::enable_power(bool shutdown) {
        // TODO: Do something
    }
}

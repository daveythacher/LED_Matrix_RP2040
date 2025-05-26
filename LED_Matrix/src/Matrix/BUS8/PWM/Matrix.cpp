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
    
    void Matrix::show(unique_ptr<::Matrix::Packet> &packet) {
        unique_ptr<::Matrix::Packet> p(get_packet());
        packet.swap(p);
        multiplex->show(static_cast<Packet *>(p.release()));   // Promote
    }
    
    unique_ptr<::Matrix::Packet> Matrix::get_packet() {
        unique_ptr<::Matrix::Packet> result(Packet::create_packet(MULTIPLEX, STEPS, COLUMNS));
        return result;
    }

    void Matrix::work() {
        multiplex->work();
    }
}

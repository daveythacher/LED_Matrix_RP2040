/* 
 * File:   main.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include <new>
#include "pico/multicore.h"
#include "Matrix/Matrix.h"
#include "System/Watchdog.h"

static Matrix::Matrix *matrix = nullptr;

static void crash() {
    System::Watchdog::crash();
}

static void core1() {
    while (matrix == nullptr);

    matrix->work();
}

int main() {    
    std::set_new_handler(crash);
    System::Watchdog::enable(100);
    multicore_launch_core1(core1);

    matrix = Matrix::Matrix::get_matrix();
    Packet *packet = matrix->get_packet();
    Matrix::Packet *p = packet.get();

    while (1) {
        for (int i = 0; i < p->num_columns(); i++) {
            for (int j = 0; j < p->num_scan(); j++) {
                for (int k = 0; k < p->num_steps(); k++) {
                    if (i == j) {
                        p->set(j, k, i, 0xFF);
                    }
                    else {
                        p->set(j, k, i, 0);
                    }
                }
            }
        }

        matrix->show(packet);
    }
}

/* 
 * File:   Command.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "Interface/Protocol/Serial/Command/Command.h"

namespace Interface::Protocol::Serial {
    ::Matrix::Packet *Command::packet = nullptr;

    Command::Command() {
        packet = ::Matrix::Matrix::get_matrix()->get_packet();
    }
}
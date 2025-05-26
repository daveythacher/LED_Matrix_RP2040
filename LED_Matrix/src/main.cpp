/* 
 * File:   main.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include <new>
#include "Interface/Node.h"
#include "Interface/Protocol.h"
#include "Matrix/Matrix.h"
#include "System/Watchdog.h"

static void crash() {
    System::Watchdog::crash();
}

int main() {    
    std::set_new_handler(crash);
    System::Watchdog::enable(100);
    Interface::Protocol::create_protocol(Interface::Node::create_node())->work();
}

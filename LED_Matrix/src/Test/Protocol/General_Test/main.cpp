/* 
 * File:   main.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include <new>
#include <memory>
#include "pico/multicore.h"
#include "Interface/Protocol/Protocol.h"
#include "System/Watchdog.h"

namespace Interface {
    class dummy_node : public Node {
        public:
            bool put_available() {
                // TODO:
                return false;
            }

            bool get_available() {
                // TODO:
                return false;
            }

            void put(uint8_t c) {
                // TODO:
            }

            uint8_t get() {
                // TODO:
                return 0;
            }
    };
}

static void crash() {
    System::Watchdog::crash();
}

static void core1() {
    // TODO:
}

int main() {    
    std::set_new_handler(crash);
    System::Watchdog::enable(100);
    multicore_launch_core1(core1);

    Interface::dummy_node *node = new Interface::dummy_node();

    Interface::Protocol::create_protocol(node)->work();
}

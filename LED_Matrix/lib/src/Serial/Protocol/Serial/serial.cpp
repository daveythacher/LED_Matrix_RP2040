/* 
 * File:   serial.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

#include "hardware/watchdog.h"
#include "Serial/Protocol/serial.h"
#include "Serial/Protocol/Serial/internal.h"
#include "Serial/Protocol/Serial/Command/Command.h"
#include "Serial/Protocol/Serial/Command/filter.h"
#include "Serial/Protocol/Serial/control_node.h"
#include "Serial/Node/data.h"

namespace Serial::Protocol {
    void start() {
        // Setup TCAM rules
        Serial::Protocol::DATA_NODE::filter::filter_setup();
    }

    // Warning host is required to obey flow control and handle bus recovery
    void __not_in_flash_func(task)() {
        static uint64_t time = time_us_64();
        Serial::Protocol::internal::STATUS status;

        // Both of these are async and can be converted to use DMA internally, in theory.
        //  These work synchronously internally.
        //      DMA could improve stability and utilization. (It could also undermine it!)
        //      DMA's value is serializing IO and increasing response time in low throughput systems.
        //      DMA is HT rather than SMT or SMP. (This represents a larger topic of how this code is put together.)
        //          We have two to three cores worth of SMP. (4 in best and 1 in worst, so I am assuming the average case.)
        //              SMP can convert to SMT very quickly if not careful!
        //          I am somewhat throwing away core 0 on IO for responsiveness. (L1 instruction would help.)
        //          I am throwing away core 1 on compute/event processing. (L2/L3 would help the IO nature of the compute.)
        //          DMA is loading down partially to hold back overloads.
        //  DMA is currently blocked by Serial::Node
        //      Currently these do not use DMA but rather work off polling
        //      In theory, I may be able to sneak an RTOS onto core 0. (I could relocate compute to this.)
        //          Like MLA, I am in a multiplexing trap without dual core. (DMA can't fix the trap.)
        //              I can escape the trap with large memory using an ISR loop. 
        //                  Just becareful of being too tight!!!
        //          I think this ends up being three threads on a dual core. (CPU plus DMA)
        //              I have to manage that ISR manually or clock it to the speed of light.
        //              Just to attempt to clarify this: (From a memory point of view)
        //                  Thread 0: IO serial protocol pollers (These run off core 0 and can have some DMA offload)
        //                      This thread can be removed also, but that is not fully supported right now.
        //                  Thread 1: Compute thread (This can be low priority on core 0)
        //                  Thread 2: Multiplexing thread (This pretty much has to run on core 1 at high priority)
        //                      This thread can be removed and replaced by a poller with PIO in theory.
        //                  Thread 3: DMA IO operations
        Serial::Protocol::CONTROL_NODE::control_node();
        status = Serial::Protocol::DATA_NODE::Command::data_node();

        // Do not block, let flow control do it's thing
        //  This is capable of being converted to use DMA internally, in theory.
        //  I am concerned by the fork/join or pipe overhead because of status message.
        if ((time_us_64() - time) >= (Serial::Node::Data::get_packet_time_us(sizeof(Serial::Protocol::internal::Status_Message)) + 1)) {
            Serial::Protocol::internal::send_status(status);
            time = time_us_64();
        }
    }
}
/* 
 * File:   serial.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */
 
#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"
#include "pico/multicore.h"
#include "BCM/config.h"

/*
    The Matrix state machine generates a lot of interrupts, MULTIPLEX * REFRESH_RATE. This can consume a decent
    number of CPU cycles in overheard. Even if the ISR is really short this could consume whole sections of the
    total cycles. (ISR overhead/latency) Fix this with lower refresh rate.
    
    The ISR rate is too higher here. I cannot make the RTOS move this fast without creating massive problems.
    The Matrix ISR is already using a lot of cycles. (25+ percent) Having the ISR wake up a high priority task 
    if viable, may increase the length of the ISR pointlessly. If the ISR starves the RTOS this could avoid the 
    issue. The RTOS could starve the ISR with critical sections which would create relaxed deadlines.
    
    RTOS and Interrupts work better for execution control and responsiveness. Super loop works better for code
    performance. However complexity of super loop is bad. Concurrency is better with multicore, FIFO and DMA.
    Assuming you have the hardware which is capable of making use of this.
    
    Luckly for me I am doing mostly bulk IO, therefore I can use DMA without CPU core for protocol. Otherwise
    I would need to use FIFO or tight super loop. Possibly multicore would be needed too.
    
    In this case it is recommended to avoid having the kernel preempt from fast timer as this would lead to 
    unneeded overhead. For serial, infrequent interrupts would be ideal. This could be done with larger payload 
    chunks. This could even support waking up a dedicated task. However a simple ping pong, high priority ISR 
    should work also.
    
    The main reason for the RTOS would be to keep the serial bus from going idle. Avoid queues if possible.
    In fact, I may use a purely async event system here. Avoid running the kernel as much as possible would
    be recommended here. The kernel will collapse the refresh rate of the Matrix.
    
    Note I am not likely to bring core 1 into this, since it is being used almost completely for computation.
    However I suspect there are some free cycles there. (Maybe 50 percent, if lucky.) The ISR rate is possibly 
    too high.
    
    Matrix DMA ISR - Runs Matrix State Machine.
    ENC28J60 DMA ISR - Wakes ETH/IP Task.
    
    Tasks: (High to low priority)
    ETH/IP Task - For keeping SPI bus active and picking out packets. Setups DMA. Wakes APP or WEB Task.
        Wake up every 1ms. (This would limit loss to around 0.1 percent.)
        Discard Web packets if they consume too many packet slots.
        Yield after discarding a certain number of packets.
    APP Task - For getting data out of packets. Pushes to Core 1.
        DMA will need to be used for copy operation.
    WEB Task - For generating web pages. Wakes ETH/IP Task.
    
    
    Alternative: (Task could be replaced by super loop)
    Serial DMA ISR - Wakes SER Task. (Highest ISR priority, no other at same priority level for byte capture.)
        Implement ping-pong buffer swap.
    Matrix DMA ISR - Runs Matrix State Machine. (Reconsider delay_ns?)
    
    Tasks: (High to low priority)
    SER Task - For keeping serial bus active. Setups DMA.
*/

static volatile bool isReady;
static test buffers[2];
static uint8_t buffer = 0;
static StaticTask_t xTaskBuffer;
static StackType_t xStack[ configMINIMAL_STACK_SIZE ];

// This could be UART/RS-485, SPI, Ethernet, Etc.
//  If Ethernet it would be interesting to know if a webserver is possible.
static void serial_task(void *arg) {
    extern void work();
    multicore_launch_core1(work);
    // TODO:
    
    while (1) {
        if (isReady) {
            multicore_fifo_push_blocking((uint32_t) &buffers[(buffer + 1) % 2]);
            isReady = false;
        }
    }
}

void serial_start() {
    xTaskCreateStatic( serial_task, "SER", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, xStack, &xTaskBuffer );
}


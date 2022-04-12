/* 
 * File:   delay_ns.S
 * Author: David Thacher
 * License: GPL 3.0
 */

.syntax unified
.cpu cortex-m0
.text
    .thumb
    
    .thumb_func
    .global delay_ns
delay_ns:
    movs r3, #0
loop:
    nop
    adds r3, #40
    cmp r0, r3
    bhi.n loop
    bx lr 

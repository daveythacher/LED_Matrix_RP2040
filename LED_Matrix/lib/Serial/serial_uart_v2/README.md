# serial_uart_v2 Documentation
This implements the UART v2 Serial Algorithm.

## Status
Requires testing.

## Overview
This is is modified version of the UART Serial Algorithm. It creates a chip select control signal for detecting the end of serial packet over UART.

A back off time is used before and after serial data transmission to ensure there is no corruption. The control signal drives an interrupt to trigger an interrupt should a misconfiguration or error occur. 

In the event of an error the packet is discarded and the serial bus is reset to begin listening again. Note the control signal is only used on the trailing edge, it has not effect on the beginning. Which means it technically could be used as a latch signal if desired.

## Interrupts
Follows standard design for Serial Algorithms.

## Core reservations
Follows standard design for Serial Algorithms.

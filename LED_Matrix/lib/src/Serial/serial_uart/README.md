# Serial UART Documentation

## Status
Work in progress

## Overview
Serial algorithm for receiving RGB frame data.

## How it works
### Event loop
Uses event loop which should not be starved. All of this is software and therefore CPU time is required. Frame loss and errors will occur if starved.

Use calculation of worst case to determine response time required by implementation. (This is real time process.) Use calculation of worst case to determine CPU usage. This approach is used on core 1 for the worker process in the calculator of the Matrix algorithms. (This is real time process.) Usage was expressed as millions of operations per second, which ensures both conditions are met. (Multiplexing is believed to be constant load.)

### Flow Control (Out of date)
Packets are divided in two, which allows a state token to be updated. This token is transmitted every 10 microseconds to the host. Host must wait for permission before sending a packet. Host is allowed to send full packet as two DMA channels are chained to capture the full packet. The event loop will deny permission to the host at the half way point. Host must wait for packet transmission to complete before looking at token.

### Error Protocol (Out of date)
This is currently implemented by the host using the state passed from the flow control. Should the flow control fail the host is to freeze. Currently this design hopes for a miracle as the watchdog does not intercept. Should a byte be dropped the host must handle the token change did not occur as expected. This recovery should be slow and careful. (Send a byte, wait and repeat.)

At some point a checksum may be added to the packets.

### Priority Regulation
Currently point to point so not handled/required.

### Addressing
Currently point to point so not handled/required.

### Reliability
Host reserves right to implement this via commands. Must use compatible commands.

### Latency Regulation
Host is responsible for regulating this according to control node global command. Control node will send command which will propogate to all nodes in a small section of time (6kHz or higher) to prevent tearing. Flow control is used in most commands of data nodes to ensure all nodes are sync'd, allowing control node to wait till the right time to release processing. Data nodes will block till ready. Should an error occur the host is capable of becoming aware and may flow error protocol if desired. Data nodes will yield pipe after certain amount of time resulting in an error visible to the host.

### Duplex
UART 0 is full duplex over token system. (Half duplex in reality.) Point to point system. Data node used for data packets/frames/tokens. All responses required will occur here. Queries can be implemented here but are not currently.

UART 1 is simplex with token system. Point to many system. Control node used for global commands mostly, but can send commands to individual nodes over global broadcast. Queries should not be implemented here. Data node state machine should provide acknowledgement.

### Framing
Protocol uses packets and flow control to manage framing.

## Interrupts
None used per design, which forbids them in Serial Algorithms.

## Core reservations
Uses core 0 super loop. Core 1 is not used per design.
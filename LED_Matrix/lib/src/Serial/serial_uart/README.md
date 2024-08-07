# Serial UART Documentation

## Status
Work in progress

## Overview
Serial algorithm for receiving RGB frame data.

## How it works
### Event loop
Uses event loop which should not be starved. All of this is software and therefore CPU time is required. Frame loss and errors will occur if starved.

Use calculation of worst case to determine response time required by implementation. (This is real time process.) Use calculation of worst case to determine CPU usage. This approach is used on core 1 for the worker process in the calculator of the Matrix algorithms. (This is real time process.) Usage was expressed as millions of operations per second, which ensures both conditions are met. (Multiplexing is believed to be constant load.)

### Flow Control
The event loop produces tokens periodically to the host. Host sends data in stages waiting for an expected response before proceeding. When an error occurs this implementation will reset the state machine and begin producing expected tokens for the host to observe. 

Every command is sent on its own id like in USB. (Alternating 0 to 1 and 1 to 0.) See Serial::UART::internal::STATUS in lib/include/Serial/serial_uart/internal.h

### Error Protocol
This is currently implemented by the host using the state passed in the flow control. Timeout is used in data node and watchdog. (Watchdog is should trip on core 0 as we use a producer/consumer blocking pipeline on core 0 and core 1.) Corruption of tokens cause the token to be lost which the host should observe and is the host is responsible for doing the right thing.

### Priority Regulation
Currently point to point so not handled/required.

### Addressing
There is only the single endpoint on the data node, however the commands/query API in theory support multiple endpoints. IDs are supported on the control node for addressing specific nodes rather than using the broadcast address.

### Reliability
Host reserves right to implement this via commands. Must use compatible commands. Optional checksum is used to detect an unreliable transmission. Error protocol also provides some detection and reliability properties.

### Error Correction and Detection
Currently there is a single parity bit. The protocol support reliable comms, if desired. ECC is not currently supported as we do believe the network is reliable enough to avoid it.

### Latency Regulation
Host is responsible for regulating this according to control node global command. Control node will send command which will propogate to all nodes in a small section of time (6kHz or higher) to prevent tearing. Flow control is used in most commands of data nodes to ensure all nodes are sync'd, allowing control node to wait till the right time to release processing. Data nodes will block till ready. Should an error occur the host is capable of becoming aware and may flow error protocol if desired. Data nodes will yield pipe after certain amount of time resulting in an error visible to the host.

### Duplex
UART 0 is full duplex over token system. (Half duplex in reality.) Point to point system. Data node used for data packets/frames/tokens. All responses required will occur here. Queries can be implemented here but are not currently.

UART 1 is simplex with token system. Point to many system. Control node used for global commands mostly, but can send commands to individual nodes over global broadcast. Queries should not be implemented here. Data node state machine should provide acknowledgement.

### Framing
Protocol uses packets and flow control to manage framing.

### Topology
UART 0 is point to point topology. Unicast only. Response are allowed.

UART 1 is point to many (bus) or point to point topology. Broadcast mostly but unicast is also supported if configured using UART 0. Responses are not allowed.

## Interrupts
None used per design, which forbids them in Serial Algorithms.

## Core reservations
Uses core 0 super loop. Core 1 is not used per design.

# Serial UART Documentation

## Status
Work in progress

## Overview
Serial algorithm for receiving RGB frame data.

## How it works
### Event loop
Uses event loop which should not be significantly starved. Most of this is software and therefore some CPU time is required. 

Do not miss the half packet mark under any circumstances. Packet size is frame size divided by two divided by multiplex. Divide this number by two and compute the packet bit time. You must never speed more than this amount of time away. 

Maybe in the future this will become a high priority interrupt to forbid this from ever creating a problem. (Higher than the multiplexing of the Matrix algorithm.)

### Flow Control
Packets are divided in two, which allows a state token to be updated. This token is transmitted every 10 microseconds to the host. Host must wait for permission before sending a packet. Host is allowed to send full packet as two DMA channels are chained to capture the full packet. The event loop will deny permission to the host at the half way point. Host must wait for packet transmission to complete before looking at token.

### Error Protocol
This is currently implemented by the host using the state passed from the flow control. Should the flow control fail the host is to freeze. Currently this design hopes for a miracle as the watchdog does not intercept. Should a byte be dropped the host must handle the token change did not occur as expected. This recovery should be slow and careful. (Send a byte, wait and repeat.)

At some point a checksum may be added to the packets.

### Priority Regulation
Currently point to point so not handled/required.

### Addressing
Currently point to point so not handled/required.

### Reliability
Not implemented and not planned.

### Latency Regulation
Frames are divided into packets which allows normalizing the time between multiple RP2040. Host is responsible for regulating this according to flow control.

### Duplex
Currently the protocol is half duplex but uses full duplex for flow control. 

In the future should full duplex be used it should be handled in protocol via half duplex token system.

### Framing
Protocol uses packets and flow control to manage framing.

## Interrupts
None used

## Core reservations
Uses core 0 super loop.
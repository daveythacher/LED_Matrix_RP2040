# Internal Documentation
Place to talk about internal code stuff.

## Overview
This code base is divided into two main parts: matrix/multiplex and interface. Not all features are implemented in this code base.

## Status
### Matrix:
PWM - experimental

SPWM - experimental

### Multiplex:
Decoder - experimental

### Interface/Node:
UART - experimental

### Interface/Protoocl
Serial - experimental

## List of things:
Planned things:
- 4-bit LUT to accelerate computation. (Moved to application code.)
- TCAM to speed up processing/filtering. (Simplified form used.)
- Portable SIMD operations using standard ALU. (Moved to application code.)
- Pointers/structures to accelerate computation. (Somewhat used.)
- Wrapper to promote in-order execution. (Not really required.)
- DMA wrapper to scope system networking. (Logic not large enough for system service.)
- Watchdog service to failsafe RTOS. (Simplified form used.)
- Tone to detect errors and possibly correct them. (High framerate with acknowledge reduced the value of this.)
- Multiple copies of bits to enable voting. (High framerate with acknowledge reduced the value of this.)
- Implementing data chunks with checksum below protocol. (Hardware flow control reduced the value of this.) 
- Commands and queries for remote control. (Implemented)
- Synchronous trigger for frame swap and refresh sync. (Implemented)
- Concurrent libraries for frame, mutex, queue, threads (RTOS), atomic, etc. (Simplification reduced the value of this.)
- Shift register based vector or ring buffer. (Somewhat used.)
- S-PWM algorithm using universal table for increasing color depth. (Moved to application logic.)


Other things:
- Hardware interface logic vs high level software (asynchronous vs dependency order) portability. (File approach used.)
- Preprocessor vs linker symbol management vs dynamic runtime. (Preprocessor used.)
- RAM vs XIP caching. (Copy to RAM used.)
- ASM vs C vs C++ (C++ where possible.)


Moved to application:
- Dot correction computation
- Brightness compuation
- Gamma computation
- Color depth computation
- Color order computation
- Mapping computation


IO protocol properties:
- Event loop
- Flow control
- Error protocol
- Priority regulation
- Addresssing
- Reliability
- Error correction and detection
- Latency regulation
- Duplex
- Framing
- Topology

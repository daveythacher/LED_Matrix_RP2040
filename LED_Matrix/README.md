# Internal Documentation
Place to talk about internal code stuff. (Note more information is available [here](https://github.com/daveythacher/LED_Matrix_RP2040/blob/main/LED_Matrix/doc/Applications.md) and [here](https://github.com/daveythacher/LED_Matrix_RP2040/blob/main/LED_Matrix/doc/Memory.md).)

## Overview
This code base is divided into two parts. The first part is applications which are front end protocols. The second part is matrix algorithms which are back end panel implementations. Not all features are implemented in this code base. For example: gamma, brightness, CIE1931, pixel mapping, color order, etc. do not belong in this code base these are handled by the application logic.

## Status
### Matrix:
PWM - experimental

SPWM - experimental

### Multiplex:
Decoder - experimental

### Serial/Node:
UART - experimental

### Serial/Protoocl
Serial - experimental

## List of things:
Planned things:
- 4-bit LUT to accelerate computation. (Removed due to direction shift in support.)
- TCAM to speed up processing/filtering. (Removed due to direction shift in support.)
- Portable SIMD operations using standard ALU. (Removed due to direction shift in support.)
- Pointers/structures to accelerate computation. (Somewhat used.)
- Wrapper to promote in-order execution. (Removed in favor of simplicity.)
- DMA wrapper to scope system networking. (Removed in favor of simplicity.)
- Watchdog service to failsafe RTOS. (Removed in favor of simplicity.)
- Tone to detect errors and possibly correct them. (High framerate with acknowledge reduced the value of this.)
- Multiple copies of bits to enable voting. (High framerate with acknowledge reduced the value of this.)
- Implementing data chunks with checksum below protocol. (Hardware flow control reduced the value of this.) 
- Commands and queries for remote control. (Planned)
- Synchronous trigger for frame swap and refresh sync. (Planned)
- Concurrent libraries for frame, mutex, queue, threads (RTOS), atomic, etc. (Removed in favor of simplicity.)
- Shift register based vector or ring buffer. (Removed ring buffer approach in favor of field extractor.)
- S-PWM algorithm using universal table for increasing color depth. (Planned)


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
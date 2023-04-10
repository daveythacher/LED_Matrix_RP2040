# USB Documentation
This implements a blocking transfer of frames between USB host and RP2040. The frame rate is not known however smaller displays should have better stability. The USB host is supposed to implement async logic to keep frame rate as desired.

## Status
Experimental and work in progress. Not finished.

## Overview
The current logic does not use USB double buffer which could improve the latency. Currently Core 0 is used for USB logic however the matrix algorithms have higher priority access over the USB. DMA is used but currently has small contribution without double buffering. If double buffering was used the DMA would allow ping pong and the CPU could work in the background. However currently this is mostly a serial operation.

Currently the way this serial operation works is suboptimial for bulk endpoints. Bulk endpoints are used since the demo code adapted was bulk. Using proper USB hubs are believed to matter. Bulk endpoints are blocking and there are two blocking points within the implementation. (USB RX and DMA TX)

The first is within the bulk endpoints. The entire frame is divided up into up to 15 bulk endpoints. The number of endpoints used required depends on the number of 64 byte chunks of the frame size. The frame size should be an even multiple of 64. All endpoints wait for a packet to arrive before DMA copies them to the RGB buffer. An endpoint stall will block DMA.

The second blocking point is within the serial implementation which handles the RGB buffer allocation. All buffer chunks must be requested before hand. Once the whole buffer has been loaded then the frame is swapped. DMA will block USB RX.

Currently there are lots of asnyc callbacks from the USB system for endpoints and interrupt. A single DMA interrupt is also used. (The event rate of these is fairly low however fair from ideal.)

## USB hubs
The RP2040 is USB full speed if using multiple RP2040s on a single USB hub it is recommended to use one which is capable of providing non-blocking USB full speed. These require special modules on the down stream ports rather than up stream port.

## Interrupts
The highest priority should go to the matrix algorithm. The next highest should be the USB DMA. Finally the USB controller interrupts at the lowest priority. Again this is a blocking systems so this should not cause to many issues. We do block in the USB ISR, if required. Currently this ISR is assigned level 2, which is fairly low priority.

## Core reservations
This follows the same design as the one outlined in the top level documentation. A sample of implementing this Serial implementation is in LED_Matrix/src/usb.

## Endpoint allocation
The endpoint allocation attempts to use a many endpoints as possible. For example if transfering 18 chunks there will be two rounds one with 15 endpoints and one with three endpoints. The lower numbered end points are used first.

## Known issue
Currently is based on 2D transfer which in this case is kind of pointless really. So this needs to be switched to 1D transfer. This may reduce the odds of the CPU blocking ping-pong performance.

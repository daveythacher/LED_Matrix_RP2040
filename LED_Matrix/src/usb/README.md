# USB Documentation
This implements a blocking transfer of frames between USB host and RP2040. The frame rate is not known however smaller displays should have better stability. The USB host is supposed to implement async logic to keep frame rate as desired. 

The current logic does not use USB double buffer which could improve the latency.
Currently Core 0 is used for USB logic however the matrix algorithms have higher priority access over the USB. DMA is used but currently has small contribution without double buffering. If double buffering was used the DMA would allow ping pong and the CPU could work in the background. However currently this is mostly a serial operation.

Currently the way this serial operation works is suboptimial for bulk endpoints. Bulk endpoints are used since the demo code adapted was bulk. Using proper USB hubs are believed to matter. Bulk endpoints are blocking and there are two blocking points within the implementation. 

The first is within the bulk endpoints. The entire frame is divided up into up to 15 bulk endpoints.
The number of endpoints used required depends on the number of 64 byte chunks of the frame size. The frame size should be an even multiple of 64 and the number of endpoints used. (The number of endpoints used is the greatest common factor possible of the nunmber of endpoints and number of 64 byte chunkks.) All endpoints wait for a packet to arrive before DMA copies them to the RGB buffer.


The second blocking point is within the serial implementation which handles the RGB buffer allocation. All buffers must be requested before hand. (Note this is currently implemented as individual stream rather than a bulk operation as it should be.) Once the whole buffer has been loaded then the frame is swapped. Currently there are lots of asnyc callbacks from the USB system for endpoints and interrupt. A single DMA interrupt is also used. (The event rate of these is fairly low however fair from ideal.)

## USB hubs
The RP2040 is USB full speed if using multiple RP2040s on a single USB hub it is recommended to use one which is capable of providing non-blocking USB full speed. 
These require special modules on the down stream ports rather than up stream port.

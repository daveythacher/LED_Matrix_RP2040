# Matrix Algorithms
## Standard Panels
The memory required by the framebuffer is basically columns times contrast (expressed in bits). For PWM this is two raised to the number of bits times columns.

The worst case memory usage is 95.3Hz refresh using 25MHz serial clock (likely ignores fanout). In this case there are 64 columns and 1:4096 contrast (12 bits). This requires 256KB per frame, which if double buffered makes the total requirement 512KB per HUB75.

This code base currently only allocates 48KB per frame and does support double buffering. This consumes a large amount of the RP2040's RAM.

The computation power must be capable of processing atleast 30 frames per second, which is believed to be possible with core 1 when the frame is 48KB.

## GCLK Panels
The memory required here is tied to the refresh rate for the most part. (Ignoring fanout.) Lower refresh rates require more memory per frame.

## GCLK Panels with SRAM
The memory required here is tied to the frame rate for the most part. (Ignoring fanout.) Lower frame rates require more memory per frame. Generally expected to saturate around 96KB per frame, but this depends on the fanout.

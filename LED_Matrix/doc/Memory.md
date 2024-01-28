# Matrix Algorithms
## Standard Panels
The memory required by the framebuffer is basically columns times contrast (expressed in bits). For PWM this is two raised to the number of bits times columns.

The worst case memory usage is 95.3Hz refresh using 25MHz serial clock (likely ignores fanout). In this case there are 64 columns and 1:4096 contrast (12 bits). This requires 256KB per frame, which if double buffered makes the total requirement 512KB per HUB75.

This code base currently only allocates 48KB per frame and does support double buffering. This consumes a large amount of the RP2040's RAM.

The computation power must be capable of processing atleast 30 frames per second, which is believed to be possible with core 1 when the frame is 48KB.

These panels support up to 8192 pixels. (The quality declines with more pixels.) Only certain configurations are capable of reaching 8192 pixels. It is recommended to avoid applications using graphics with these panels unless the limitations are fully understood.

## GCLK Panels
The memory required here is tied to the refresh rate for the most part. (Ignoring fanout.) Lower refresh rates require more memory per frame.

These panels support up to 8192 pixels. (The quality declines with more than 64 pixels.) Only certain configurations are capable of reaching 8192 pixels. It is recommended to avoid applications using graphics with these when using lower quality configurations unless the limitations are fully understood.

## GCLK Panels with SRAM
The memory required here is tied to the frame rate for the most part. (Ignoring fanout.) Lower frame rates require more memory per frame. Generally expected to saturate around 96KB per frame, but this depends on the fanout.

These panels support up to 8192 pixels. (The quality declines with more than 256 pixels.) Only certain configurations are capable of reaching 8192 pixels. It is recommended to avoid applications using graphics with these when using lower quality configurations unless the limitations are fully understood.

# Application Information
Section documenting various application information.

## Panel Selection
Good luck with this as most vendors provide little information with this. Asking questions via message is also not to be trusted.

### Size
Ideal sizes for message boards are P6 1:8 and P5 1:16. These have support brackets which prevent LEDs from being damaged.

#### Fan out
Long chains will create issues and are limited by memory. Serial bandwidth does not scale well against pixel count due to fan out of clock lines of many panels. There are compile time calculators which help spot this. Reducing chain length, clock speed and refresh/constrast generally help.

### Refresh
High refresh displays need special control circuits in multiplexing and LED drivers. This is hard to find in P6, but can be found in P5, P4, P3, P2.5, etc. Most panels are capable of 100-240Hz which should not show flicker to many people. However some people will want higher refreshes for things like movement, cameras, headaches, flicker, etc. According to lighting studies LED Matrixes should target 1.25kHz to avoid most flicker issues. 3kHz is assumed to be flicker free and has no observable adverse consequences.

Generally high refresh rate is generally accomplished using hardware PWM. This is more efficient in terms of bus cycles. Traditional PWM is used in many cases however this is being replaced somewhat by S-PWM. The LEDs are only capable of so much contrast, so there is no point to using more PWM bits than required. These cycles can be converted to refresh rate instead.

### S-PWM
Note I discourage S-PWM due to cameras picking up distortion. Note this only really matters if you plan to use high refresh rate. Many people will not see a difference but cameras may. See internal documentation [here](https://github.com/daveythacher/LED_Matrix_RP2040/blob/main/LED_Matrix/doc/Applications.md) for more information.

### Ghosting
Refresh rate or multilexing rate is limited by certain things. The panel itself is only capable of moving so fast before small capacitors inside the panel start to slow and corrupt things. To get around this you need to use pull-ups on the low side or near the LED drivers and pull-downs on the high side or near the row drivers. If you do this the LEDs will be reverse biased when they are supposed to be off and forward biased when they are supposed to be on. (The other option is push-pull drivers.)

See internal documentation [here](https://github.com/daveythacher/LED_Matrix_RP2040/blob/main/LED_Matrix/doc/Applications.md) for more information.

## Mappers
All of this is handled by application logic.

### Color Temperature (Gamma)
Note all color mapping is handled via application logic. All values are mapped to RGB-24, RGB-48, RGB-555 or RGB-222. The firmware configuration will then decide how many of those bits can be used.

### Dot Correction
Dot correction is used to adjust for slight imperfections in LEDs, LED drivers, etc.

Note it would not be recommended to use less than 5-bits of dot correction. It should also be noted that you are free to not use dot correction. This is a decision made by the application logic which is passed thru.

### Multiplexer
Pixel locations within panels is handled by application logic.

### Pixel
Panels mappings and orrientation of panels is handled by application logic. These are likely difficult to support so these should be an independent module of the application logic.

### RGB locations
Color locations within pixels is handled by application logic.

### Brightness
Handled by application logic.

### Color Depth
LEDs only support 11-13 bits of color depth. Multiplexing and power scaling reduce this. What is left over is available to creating different colors in panel. Note distance and/or dayligh is also believed to reduce color depth.

For example four 16x32 P6 1:8 panels given 11 bit color depth LEDs would have a max color depth of 6 bits if the power was limited to 9.6 Watts compared to 38.4 Watts. (11 - log2(4) - log2(8) = 11 - 2 - 3 = 6)

## Power supply
It is important to use a quality power supply with proper voltage rating, load regulation, current capacity, power supply response time, etc.

## Wiring and Signal issues
See internal documentation [here](https://github.com/daveythacher/LED_Matrix_RP2040/blob/main/LED_Matrix/doc/Applications.md) for more information.

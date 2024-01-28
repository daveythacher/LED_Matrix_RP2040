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

Generally high refresh rate is generally accomplished using hardware PWM. This is more efficient in terms of bus cycles. Traditional PWM is used in many cases however this is being replaced somewhat by S-PWM. The LEDs are only capable of so much contrast, so there is no point to using more PWM bits than required. These cycles can be converted to refresh rate instead. S-PWM enables automated configuration and smoother frame changes. S-PWM makes more sense for hardware than it does for software. Mostly used for anti-ghosting for certain hardware configurations. 

BCM is used for software traditional PWM to emulate hardware traditional PWM. BCM is computationally better than PWM. S-PWM is not used as configuration can scale the refresh and PWM bits manually. BCM may have issues with duty cycle or brightness at high refresh rates.

### S-PWM
Note I discourage S-PWM due to cameras picking up distortion. (Note this only really matters if you plan to use high refresh rate.) Many people will not see a difference but cameras may. (Backed out experimental Scrambled Matrix Algorithms due to this.) See internal documentation [here](https://github.com/daveythacher/LED_Matrix_RP2040/blob/main/LED_Matrix/Applications.md) for more information.

### Ghosting

Refresh rate or multilexing rate is limited by certain things. The panel itself is only capable of moving so fast before small capacitors inside the panel start to slow and corrupt things. To get around this you need to use pull-ups on the low side or near the LED drivers and pull-downs on the high side or near the row drivers. If you do this the LEDs will be reverse biased when they are supposed to be off and forward biased when they are supposed to be on. (The other option is push-pull drivers.)

Only certain LED panel components offer these features, however board designers can usually add them on. The larger these capacitors the longer the time required to charge them or the stronger the pull-ups/downs need to be. A careful balance between the two is required. Power consumption can become an issue if pull-ups/downs are too strong and lower refreshes are possible if the charge time is too high. This code base provides some support in GEN 1 panels for charge time using OE signal, blanking time and disabling outputs using serial data.

Another issue than can occur is the response time of LEDs themselves. If the LEDs have large capacitors on the low side there may be intensity corruption if you attempt to send a signal into it too fast. This will play a larger role on least significant PWM bits as these may be attenuated completely. Therefore large PWM periods may be recommended to combat this. BCM in this case is not recommended as it simulates a small sub PWM periods like what is used in S-PWM. These are attentuated independently, as the state change is what created the issue. PWM uses a single duty cycle, while the others use multiple.

Overall LED intensity corruption is not expected to be a huge deal using ON/OFF, low refresh BCM or low color depth PWM on GEN 1 panel. GEN 2 panels are also less likely to be an issue. GEN 3 requires carefuly consideration as mentioned in S-PWM section. 

To compensate for blanking time increase serial clock or grayscale clock by at least 30 percent. (Rough guideline which is not correct in all cases.)

See internal documentation [here](https://github.com/daveythacher/LED_Matrix_RP2040/blob/main/LED_Matrix/Applications.md) for more information.

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
See internal documentation [here](https://github.com/daveythacher/LED_Matrix_RP2040/blob/main/LED_Matrix/Applications.md) for more information.

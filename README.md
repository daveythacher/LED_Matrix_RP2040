# LED_Matrix_RP2040
This code base builds a hardware module for abstracting multiplexing of LED matrix. The implementation is a memory mapped controller based on RP2040. 24-bit RGB data is sent over an IO bus to the RP2040 which is converted into the correct format for the LED display. This data is then replayed to the LED matrix repeatedly. The purpose of this project is to enable high efficiency, high determinism  and low cost for maximum refresh, density and/or color depth potential. These are enabled by the RP2040's unique hardware configuration.

Pixel mapping, brightness, color temperature/gamma, dot correction, scaling, etc. is handled by the application which produces 24-bit RGB data over the IO bus. Note IO bus may increase latency of the display. The IO bus will likely limit the size and frame rate of the display. The RP2040 has a decent amount of resources however performance tradeoffs are expected due to processing power, IO bus bandwidth/latency, memory, and HUB75 serial bandwidth. This project was originally created for a small PoE display. It is in no way capable of competing against a video wall and represents a smaller version of receiver card, in theory.

Static configuration is used but a configuration file exists which makes adjusting this fairly straightforward. Below there are some timing equations to help guide the configuration. Compile time checks will attempt to verify this configuration also. Note you are still required to perform required testing as these only help with certain issues.

There is no intention of ever supporting anything but the RP2040 or derivatives of the RP2040. Emphasis is on the RP2040 alone, so no FPGA coprocessors. Design for the project to be a hardware module/library. Standalone operation is possible, but this is outside the scope of this project. Interfacing will be kept flexible to allow a wide range of integrations.

## Status
This code base is a work in progress. Some sections are still experimental. See [this](https://github.com/daveythacher/LED_Matrix_RP2040/blob/main/LED_Matrix/README.md#status) for more details.

I am not responsible in any way for any damages or issues created in any way as a result of this code base. This code base is licensed under GPL 3 and the terms of that are expected to be upheld. Credit where credit is due is also required.

## Contributing, feedback, questions, etc.
If in doubt use the discussion feature. I am okay with meaningful discussion in issues however please use the discussion feature first.

Some weird conventions are used. These should be documented in LED_Matrix/README.md. This is the place for documenting technical interworkings and troubleshooting. (Outside of comments.)

## Setup
For Linux: (Will install dependencies and download code)
```bash
./setup.sh
```

## Building
For Linux:

```bash
groovy build.groovy -c cfg.xml
./LED_Matrix/build/build.sh
```

For generating doxygen documentation:
```bash
doxygen Doxyfile
```

## Configuration
See [this](https://github.com/daveythacher/LED_Matrix_RP2040/blob/main/Configuration.md) for more details.

For adding or disabling flavors, see cfg.xml. Flavor configuration blocks looks like this:
``` XML
<cfg>
    <build name="P4-BCM" enable="true" app="uart" alogrithm="BCM" fps="30" multiplex="16" multiplex_name="Direct" max_rgb_led_steps="1000" max_refresh="190" columns="128" serial_clock="25.0" blank_time="1"/>
    <!-- ... -->
</cfg>
```

## Panel Selection

Good luck with this as most vendors provide little information with this. Asking questions via message is also not to be trusted.

### Size

Ideal sizes for message boards are P6 1:8 and P5 1:16. These have support brackets which prevent LEDs from being damaged.

### Refresh

High refresh displays need special control circuits in multiplexing and LED drivers. This is hard to find in P6, but can be found in P5, P4, P3, P2.5, etc. Most panels are capable of 100-240Hz which should not show flicker to many people. However some people will want higher refreshes for things like movement, cameras, headaches, flicker, etc. According to lighting studies LED Matrixes should target 1.25kHz to avoid most flicker issues. 3kHz is assumed to be flicker free and has no observable adverse consequences.

Generally high refresh rate is generally accomplished using hardware PWM. This is more efficient in terms of bus cycles. Traditional PWM is used in many cases however this is being replaced somewhat by S-PWM. S-PWM allows you to lower the color depth aka PWM bits in response to multiplex or scan rate. The LEDs are only capable of so much contrast, so there is no point to using more PWM bits than required. These cycles can be converted to refresh rate instead. S-PWM enables this and smoother frame changes. S-PWM makes more sense for hardware than it does for software. Mostly used for anti-ghosting for certain hardware configurations.

BCM is used for software traditional PWM to emulate hardware traditional PWM. BCM is computationally better than PWM. S-PWM is not used as configuration can scale the refresh and PWM bits manually. BCM may have issues with duty cycle or brightness at high refresh rates.

## Power consumption

LEDs generally support 11-13 bits of current division. This is the max color depth possible. Multiplexing increases current division which lowers color depth. The same is true for power scaling to support PoE and/or battery applications.

### Ways to lower power consumption

1. Reduce peak power by lowering the LED current via LED driver resistor. Can lower LED voltage to save more power, see Color Temperature. This does require soldering and requires a little bit of knowledge about datasheets, circuits and diodes.
2. Reduce peak power by lowering LED current via software current gain of LED driver resistor. Can lower LED voltage to save more power, see Color Temperature. This does not require soldering but does have limits. (Works better if this is implemented using analog instead of PWM.)
3. Reduce average power by controlling ON time (duty cycle). This requires bulk capacitors and probably an inrush limiter. Cannot lower LED voltage to save more power, must use 5V.
4. Reduce average/peak power by controlling the number of LEDs on at any given time. This gets tricky and is probably not recommended. There are two ways to do this: time sharing panels and modifying bitplanes.
5. Reduce average/peak power with current limiter. This is advanced and will not work on certain LED drivers.

You can further lower power consumption by lowering power supply voltage. As the LED current is reduced the require forward voltage is also reduced. However this can lead to problems for color temperature. Note some LED drivers have min constant current limits.

### Color Temperature

As the LED current is reduced the required forward voltage is also reduced. This allows for the overall voltage of the LED power rail to be less. Down to around 3.3V. When lowering the peak power consumption this is generally provided for free. Note this does not apply to average power consumption.

Messing this up can lead to issues for color temperature of white. The color may be too warm or too cool. Current balancing to the get the color temperature may be required. Note some LEDs are different. There can also be differences in performance between LED colors. Manipulate the current for the peak (full) current to get the color temperature correct. Then adjust the color intensity using gamma curves per color or dot correction.

CIE1931 is another feature which can be used if desired. Note all color mapping is handled via application logic. All values are mapped to RGB-24 and the firmware configuration will then decide how many of those bits can be used.

## Color Depth

LEDs only support 11-13 bits of color depth. Multiplexing and power scaling reduce this. What is left over is available to creating different colors in panel.

For example four 16x32 P6 1:8 panels given 11 bit color depth LEDs would have a max color depth of 6 bits if the power was limited to 9.6 Watts. 

30mA * 32 columns * 16 rows / 8 scan * 4 panels * 5V = 38.4 Watts / 9.6 Watts = 4.

11 - log2(4) - log2(8) = 11 - 2 - 3 = 6

Now lets consider lowering the power consumption to 6.8 Watts using the three approaches listed above. Here are the following bit depths:

1. 8mA * 32 columns * 16 rows / 8 scan * 4 panels * 3.3V = 6.8 Watts. 11 - roundup(log2(30 / 8)) - log2(8) = 6 bits
2. 30mA * .25 current gain * 32 columns * 16 rows / 8 scan * 4 panels * 3.3V = 6.4 Watts. 11 - roundup(log2(1 / .25)) - log2(8) = 6 bits
3. 30mA * 32 columns * 16 rows / 8 scan * 4 panels * 5V = 38.4 Watts. 11 - roundup(log2(38.4 / 6.8)) - log2(8) = 5 bits

### Dot Correction

Dot correction is used to adjust for slight imperfections in LEDs, LED drivers, etc. Generally it will double the amount of color depth required. Meaning that you should be able to scale the LED current between 0.5 and 1 times the configured limit. Having 5-bits of color depth would also mean you would have 5-bits of dot correction. This means PWM_bits would need to be configured for 6-bits. 

Note it would not be recommended to use less than 5-bits of dot correction. It should also be noted that you are free to not use dot correction. This is a decision made by the application logic. This logic should be considered experimental and this serves as a reminder for application logic. No real work has been put into it yet.

## PoE Display
16x128 using MBI5124 drivers with about 3-4 bits PWM at 1-3kHz refresh. This should allow decent colors and messaging flexibility. Thirty frames per second will be supported by the main controller, which should enable animations to work smoothly.

LEDs look to be capable of 12 bit, however to make it work with PoE I had to reduce this by a factor of four. The panels are eight scan panels, which lowers this by a factor of eight. Factoring in viewing distance of twelve feet this lowers it by a factor of sixteen. Factoring in dot correction this increases it by a factor of two. The resulting PWM bit count is four, but only three are used for color temperature.

MBI5124 supports low side anti-ghosting and supports low current stability which is important for this PoE display. High refresh rate is used to avoid flicker since this ON/OFF PWM. A 3.3V supply rail is used to save power, allowing this to work with within the PoE powered device limit of 12.95W as defined by 802.3af.

The display is formed using four 16x32 RGB LED panels. Each panel will have its own RP2040 running this code base. (See configuration POE in cfg.xml for configuration details.) These panels are standard LED drivers (GEN 1 drivers) outside of the features provided by MBI5124 for refresh and low power. There is likely no benefit in this case for hardware PWM (GEN 2 drivers) or hardware PWM with SRAM (GEN 3 drivers). Panels based on those drivers should also work, however GEN 1 drivers using multiple chains is likely good enough. GEN 3 may support using fewer chains, however finding panels using GEN 3 drivers is harder at this density and cost.

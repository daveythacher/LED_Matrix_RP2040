# LED_Matrix_RP2040
This code base builds a hardware module for abstracting multiplexing of LED matrix. The implementation is a memory mapped controller based on RP2040. 24 bit RGB data is sent over an IO bus to the RP2040 which is converted into the correct format for the LED display. This data is then replayed to the LED matrix repeatedly.

Pixel mapping is handled by the application which produces 24 bit RGB data over the IO bus. Note IO bus may increase latency of the display. The IO bus will likely limit the size and frame rate of the display. The RP2040 has a decent amount of resources however performance tradeoffs are expected due to processing power, IO bus bandwidth/latency, memory, and HUB75 serial bandwidth. This project was originally created for a small PoE display. It is in no way capable of competing against a video wall and represents a smaller version of receiver card, in theory.

Static configuration is used but a configuration file exists which makes adjusting this fairly straightforward. Below there are some timing equations to help guide the configuration. Compile time checks will attempt to verify this configuration also. Note you are still required to perform required testing as these only help with certain issues.

## Status
This code base is a work in progress. Some sections are still experimental.

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

For adding or disabling flavors, see cfg.xml. Flavor configuration blocks looks like this:
``` XML
<cfg>
    <build name="P4-BCM" enable="true" app="uart" alogrithm="BCM" fps="30" multiplex="16" multiplex_name="Direct" max_rgb_led_steps="1000" max_refresh="190" columns="128" serial_clock="25.0" blank_time="1"/>
    <!-- ... -->
</cfg>
```

For generating doxygen documentation:
```bash
doxygen Doxyfile
```

## Configuration
### name 
This is the name given to the configuration and corresponding binary. Note binary will have the following prefix: led_

This name must be different from the algorithm. Currently this means something other than the following: BCM, PWM and TLC5958

### enable 
This must be true or false. If false this build will not be included in the build script.

### app
This is a string for the corresponding application in src folder. These applications determine the serial implementation protocol.

### algorithm
This is the name of the LED panel driver or algorithm used to talk to the panel. 

GEN 1 panels (standard panels) currently use BCM. (FM612x are not supported currently. Traditional PWM requires external FPGA, which is not supported by this code base.)

GEN 2 panels (hardware PWM panels without memory) are not supported currently. 

GEN 3 panels (hardware PWM panels with memory) can only be TLC5958 (incomplete) currently.

### fps
This is the number of FPS desired by GEN 3 panels. This is used to verify the serial clock requirements.

### multiplex
This is the scan number marked on the back of the panel. This number is usually in the middle near a S prefix.

### multiplex_name
This is the name for multiplexing approach used. Currently available are Direct and Decoder. Direct is more common in low multiplex.

### max_rgb_led_steps
This is the number of uA's supported by the LEDs without multiplexing. (This is generally something along order of 2000-8000.) Assuming the LED is capable of lighting up slightly at 2uA and the min constant forward current of the red, green and blue colors is 8mA. You should have 4000 steps or support around 12 bits of PWM if the panel was single scan.(8mA / 2uA = 4000)

The library will determine the max number of PWM bits from this number. By dividing this number by the multiplex and taking the log2 of the result. Note if you lower the forward current you should change this value to avoid wasting serial bandwidth and memory. The compiler will check for errors if this is set to an unsupported value. There is only so much memory on the RP2040, so lowering this may be required. This lowers the color depth on the device. Note this number should be whole numbers only.

### max_refresh
This number is the refresh rate in Hz of the panels multiplexing. When using BCM this will override the FPS. Note this number should be whole numbers only.

### columns
This is the number of real columns in the panel. Not the number of columns you see in the panel. If you have 16x32 with 4 scan panel you will need to set this to 64. panel_rows / (2 * scan) * panel_columns. Mapping of pixel location is not handled by the RP2040, this should be done in application logic or by logic driving serial bus. Note this number should be whole numbers only.

### serial_clock
This is the target serial bandwidth, in MHz. This is used by the compiler to verify the timing. This should not exceed 25MHz for most panels. Note you may wish to lower this is in some cases to meet timing and/or promote signal stability. (Measure rise/fall time, hold time, etc.) Note this number can have decimals.

### blank_time
This is the number of uS the LEDs will be off during multplexing to prevent ghosting. This is usually 1-4uS. Note this number should be whole numbers only.

### red_gain
Constant current gain value for Red LED. This value is constant multiple of the default gain. 1.0 configures the gain to be the default gain. Note this number can have decimals. (Only used/needed by TLC5958.)

### green_gain
Constant current gain value for Green LED. This value is constant multiple of the default gain. 1.0 configures the gain to be the default gain. Note this number can have decimals. (Only used/needed by TLC5958.)

### blue_gain
Constant current gain value for Blue LED. This value is constant multiple of the default gain. 1.0 configures the gain to be the default gain. Note this number can have decimals. (Only used/needed by TLC5958.)

### Timing algorithm for BCM and PWM:

serial_clock / (multiplex * columns * max_refresh * 2^round(log2(max_rgb_led_steps / multiplex))) >= 1.0

### Timing algorithm for TLC5958: - WIP

(serial_clock * 0.75) / (multiplex * columns * fps * 16 * 3) >= 1.0

serial_clock / (multiplex * max_refresh * 2^max(round(log2(max_rgb_led_steps / multiplex)), seg_bits)) >= 1.0

## Panel Selection

Good luck with this as most vendors provide little information with this. Asking questions via message is also not to be trusted.

### Size

Ideal sizes for message boards are P6 1:8 and P5 1:16. These have support brackets which prevent LEDs from being damaged.

### Refresh

High refresh displays need special control circuits in multiplexing and LED drivers. This is hard to find in P6, but can be found in P5, P4, P3, P2.5, etc. Most panels are capable of 100-240Hz which should not show flicker to many people. However some people will want higher refreshes for things like movement, cameras, headaches, flicker, etc.

Generally high refresh rate is generally accomplished using hardware PWM. This is more efficient in terms of bus cycles. Traditional PWM is used in many cases however this is being replaced somewhat by S-PWM. S-PWM allows you to lower the color depth aka PWM bits in response to multiplex or scan rate. The LEDs are only capable of so much contrast, so there is no point to using more PWM bits than required. These cycles can be converted to refresh rate instead. S-PWM enables this and smoother frame changes. S-PWM makes more sense for hardware than it does for software. Mostly used for anti-ghosting for certain hardware configurations.

BCM is used for software traditional PWM to emulate hardware traditional PWM. BCM is computationally better than PWM. S-PWM is not used as configuration can scale the refresh and PWM bits manually.

Hardware PWM panels are not supported currently. These could be added later on, however many premade panels do not have fully documented datasheets available. Making panels using other drivers like TI drivers can get fairly costly. Hardware PWM drivers come in two flavors, with and without memory. Those without memory consume cycles for high refresh rates as they lack the memory required to avoid multiplexing over the bus. Hardware PWM drivers use a different data format, PIO should provide good support for those without lots of computational overhead.

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

Messing this up can lead to issues for color temperature of white. The color may be too warm or too cool. Current balancing to the get the color temperature may be required. Note some LEDs are different. There can also be differences in performance between LED colors. Manipulate the current for the peak (full) current to get the color temperature correct. Then adjust the color intensity using gamma curves per color.

CIE1931 is another feature which can be used if desired. Note all color mapping is handled via lookup table called index_table. This table is created by the application logic and send over the serial interface before receiving RGB pixel data. This allows the application logic to have full control over this and pixel locations.

## Color Depth

LEDs only support 11-13 bits of color depth. Multiplexing and power scaling reduce this. What is left over is available to creating different colors in panel.

For example four 16x32 P6 1:8 panels given 11 bit color depth LEDs would have a max color depth of 6 bits if the power was limited to 9.6 Watts. 

30mA * 32 columns * 16 rows / 8 scan * 4 panels * 5V = 38.4 Watts / 9.6 Watts = 4.

11 - log2(4) - log2(8) = 11 - 2 - 3 = 6

Now lets consider lowering the power consumption to 6.8 Watts using the three approaches listed above. Here are the following bit depths:

1. 8mA * 32 columns * 16 rows / 8 scan * 4 panels * 3.3V = 6.8 Watts. 11 - roundup(log2(30 / 8)) - log2(8) = 6 bits
2. 30mA * .25 current gain * 32 columns * 16 rows / 8 scan * 4 panels * 3.3V = 6.4 Watts. 11 - roundup(log2(1 / .25)) - log2(8) = 6 bits
3. 30mA * 32 columns * 16 rows / 8 scan * 4 panels * 5V = 38.4 Watts. 11 - roundup(log2(38.4 / 6.8)) - log2(8) = 5 bits


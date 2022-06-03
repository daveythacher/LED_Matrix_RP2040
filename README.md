# LED_Matrix_RP2040

## Building
For Linux:

```bash
groovy build.groovy -c cfg.xml
./LED_Matrix/build/build.sh
```

For adding or disabling flavors, see cfg.xml. Flavor configuration blocks looks like this:
``` XML
<cfg>
    <build name="P4-BCM" enable="true" app="usb" multiplex="16" multiplex_num="0" max_rgb_led_steps="1000" max_refresh="190" fps="30" columns="128" serial_clock="25.0" blank_time="1" power_divisor="1" use_cie1931="1"/>
    <!-- ... -->
</cfg>
```

## Configuration
### name 
This is the name given to the configuration and corresponding binary. Note binary will have the following prefix: led_

### enable 
This must be true or false. If false this build will not be included in the build script.

### app
This is a string for the corresponding application in src folder. These applications determine the serial implementation protocol, if used and LED Matrix algorithm. usb and spi are BCM. These are only supported for standard LED driver panels. app3 is for MBI5153 LED driver panels only. Other application specific options also exist.

### multiplex
This is the scan number marked on the back of the panel. This number is usually in the middle near a S prefix.

### multiplex_num
This is a number for multiplexing approach used. 0 is for decoder based pin mapping, used in standard implemenations. 1 is for direct pin mapping, used in low multiplex panels.

### max_rgb_led_steps
This is the number of uA's supported by the LEDs without multiplexing. This is generally something along order of 2000-4000. However this assumes the LED is capable of lighting up slightly at 2uA. It also assumes that the min constant forward current of the red, green and blue colors is 8mA. 8mA / 2uA = 4000. The library will determine the max number of PWM bits from this number. By dividing this number by the multiplex and taking the log2 of the result. Note if you lower the forward current you should change this value to avoid wasting serial bandwidth and memory. The compiler will check for errors if this is set to an unsupported value. There is only so much memory on the RP2040, so lowering this may be required. This lowers the color depth on the device. Note this number should be whole numbers only.

### max_refresh
This number is the refresh rate in Hz of the panels multiplexing. When using BCM this will override the FPS. This number should never exceed 3840Hz for most panels. Note this number should be whole numbers only.

### fps 
This is the number of frames sent per second. This is used for compile timing verfication of timing in MBI5153 LED Matrix algorithm. This number should reflect the max number of FPS you plan to use. Note this reserves serial bandwidth to support this. This setting does not exist for BCM LED Matrix algorithm. Note this number should be whole numbers only.

### columns
This is the number of real columns in the panel. Not the number of columns you see in the panel. If you have 16x32 with 4 scan panel you will need to set this to 64. panel_rows / (2 * scan) * panel_columns. Mapping of pixel location is not handled by the RP2040, this should be done in application logic or by logic driving serial bus. Note this number should be whole numbers only.

### serial_clock
This is the target serial bandwidth, in MHz. This is used by the compiler to verify the timing. This should not exceed 25MHz for most panels. Note you may wish to lower this is in some cases to meet timing and/or promote signal stability. Note this number can have decimals.

### blank_time
This is the number of uS the LEDs will be off during multplexing to prevent ghosting. This is usually 1-4uS. Note this number should be whole numbers only.

### power_divisor
This can be used for brightness control. It turns the LEDs off prematurely by dividing the on time by this number. Note this will reduce the average power, but full peak power is still used. 100 percent / 1 = 100 percent. Note this number should be whole numbers only.

### use_cie1931
This enables or disables the use of the CIE1931 gamma correction table. To enable set to 1, otherwise set to 0.

### Timing algorithm for BCM:

serial_clock / (multiplex * columns * max_refresh * 2^round(log2(max_rgb_led_steps / multiplex))) >= 1.0

### Timing algorithm for MBI5153:

WIP - TBD

## Panel Selection

Good luck with this as most vendors provide little information with this. Asking questions via message is also not to be trusted.

### Size

Ideal sizes for message boards are P6 1:8 and P5 1:16. These have support brackets which prevent LEDs from being damaged.

### Refresh

High refresh displays need special control circuits in multiplexing and LED drivers. This is hard to find in P6, but can be found in P5, P4, P3, P2.5, etc. Most panels are capable of 100Hz which should not show flicker to many people. However some people will want higher refreshes. 

Controller algorithm comes into play here. LED panels only support so much serial bandwidth. Traditional PWM implemented with or without BCM generally yields lower refreshes and consumes more serial bandwidth. Computationally this can be very expensive, however BCM and SIMD can accelerate this. Scrambled PWM (S-PWM) allows for higher refreshes and more color depth for the same serial bandwidth. Computationally this is even more expensive, however using large memory look up tables can accelerate this. S-PWM is more tricky to configure as you have to consider FPS in datastream, which is generally not an issue for traditional PWM. 

S-PWM supports trading color depth for refresh dynamically. If you change the frame frequently the color depth will be reduced, however for lower frame rates the color depth will increase. The loss of color depth is fairly small, however the refresh rate can be very high. You need hardware PWM for this to work. This is not very useful over standard LED drivers, however is possible if you fully support the algorithm. It's impact is just far more limited.

Some panels use hardware PWM which is similar to traditional PWM however the computation require is reduced. Serial bandwidth is problematic still due to multiplexing. Note versions of these hardware PWM drivers also exist with S-PWM helpers built in, these still struggle with multiplexing and serial bandwidth however can be useful in certain applications. Note you would need variable FPS on serial bus via VSYNC command.

Some panels use memory with hardware PWM which is similar to controller or receiver card buit in. These are more expensive in small displays but cheaper in larger displays. In some applicaitons these can also be cheaper for example if using a Pi which allows for better stability without extra components. These panels are generally higher refresh and color depth. These generally work for high color depth in single scan with long chains (expensive) and high pixel density panels/chains.

### Power consumption

LEDs generally support 11-13 bits of current division. This is the max color depth possible. Multiplexing increases current division which lowers color depth. The same is true for power scaling to support PoE and/or battery applications.

#### Ways to lower power consumption

1. Reduce peak power by lowering the LED current via LED driver resistor. Can lower LED voltage to save more power, see Color Temperature. This does require soldering and requires a little bit of knowledge about datasheets, circuits and diodes.
2. Reduce peak power by lowering LED current via software current of LED driver resistor. Can lower LED voltage to save more power, see Color Temperature. This does not require soldering but does have limits.
3. Reduce average power by controlling ON time (duty cycle). This requires bulk capacitors and probably an inrush limiter. Cannot lower LED voltage to save more power, must use 5V.
4. Reduce average/peak power by controlling the number of LEDs on at any given time. This gets tricky and is probably not recommended. There are two ways to do this.
5. Reduce average/peak power with current limiter. This is advanced and will not work on certain LED drivers.

You can further lower power consumption by lowering power supply voltage. As the LED current is reduced the require forward voltage is also reduced. However this can lead to problems for color temperature. 

#### Color Temperature

As the LED current is reduced the require forward voltage is also reduced. This allows for the overall voltage of the LED power rail to be less. Down to around 3.3V. When lowering the peak power consumption this is generally provided for free. However to clarification purposes it is mentioned explictly. Note this does not apply to average power consumption.

However this can lead to problems for color temperature. This can be done two ways. The first is to change the LED driver resistor to be a reflection of the required current limit. The other is to program the software current gain to be a reflection of the required current limit. If this is not done the red LED is generally brighter than it is supposed to be.

There is way to do this in software with gamma curve but it reduces color depth significantly.

### Color Depth

LEDs only support 11-13 bits of color depth. Multiplexing and power scaling reduce this. What is left over is available to creating different colors in panel.

For example four 16x32 P6 1:8 panels given 11 bit color depth LEDs would have a max color depth of 6 bits if the power was limited to 9.6 Watts. 

30mA * 32 columns * 16 rows / 8 scan * 4 panels * 5V = 38.4 Watts / 9.6 Watts = 4.

11 - log2(4) - log2(8) = 11 - 2 - 3 = 6

Now lets consider lowering the power consumption to 6.8 Watts using the three approaches listed above. Here are the following bit depths:

1. 8mA * 32 columns * 16 rows / 8 scan * 4 panels * 3.3V = 6.8 Watts. 11 - roundup(log2(30 / 8)) - log2(8) = 6 bits
2. 30mA * .25 current gain * 32 columns * 16 rows / 8 scan * 4 panels * 3.3V = 6.4 Watts. 11 - roundup(log2(1 / .25)) - log2(8) = 6 bits
3. 30mA * 32 columns * 16 rows / 8 scan * 4 panels * 5V = 38.4 Watts. 11 - roundup(log2(38.4 / 6.8)) - log2(8) = 5 bits




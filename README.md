# LED_Matrix_RP2040

## Panel Selection

Good luck with this as most vendors provide little information with this. Asking questions via message is also not to be trusted.

### Size

Ideal sizes for message boards are P6 1:8 and P5 1:16. These have support brackets which prevent LEDs from being damaged.

### Refresh

High refresh displays need special control circuits in multiplexing and LED drivers. This is hard to find in P6, but can be found in P5, P4, P3, P2.5, etc. Most panels are capable of 100Hz which should not show flicker to many people. However some people will want higher refreshes. 

Controller algorithm comes into play here. LED panels only support so much serial bandwidth. Traditional PWM implemented with or without BCM generally yields lower refreshes and consumes more serial bandwidth. Computationally this can be very expensive, however BCM and SIMD can accelerate this. Scrambled PWM (S-PWM) allows for higher refreshes and more color depth for the same serial bandwidth. Computationally this is even more expensive, however using large memory look up tables can accelerate this. S-PWM is more tricky to configure as you have to consider FPS in datastream, which is generally not an issue for traditional PWM.

Some panels use hardware PWM which is similar to traditional PWM however the computation require is reduced. Serial bandwidth is problematic still due to multiplexing. Note versions of these hardware PWM drivers also exist with S-PWM helpers built in, these are still struggle with multiplexing and serial bandwidth however can be useful in certain applications.

Some panels use memory with hardware PWM which is similar to controller or receiver card buit in. These are more expensive in small displays but cheaper in larger displays. In some applicaitons these can also be cheaper for example if using a Pi which allows for better stability without extra components. These panels are generally higher refresh and color depth. These generally work for high color depth in single scan with long chains (expensive) and high pixel density panels/chains.

### Power consumption

LEDs generally support 11-13 bits of current division. This is the max color depth possible. Multiplexing increases current division which lowers color depth. The same is true for power scaling to support PoE and/or battery applications.

#### Ways to lower power consumption

1. Reduce peak power by lowering the LED current via LED driver resistor. Can lower LED voltage to save more power, see Color Temperature. This does require soldering and requires a little bit of knowledge about datasheets, circuits and diodes.
2. Reduce peak power by lowering LED current via software current of LED driver resistor. Can lower LED voltage to save more power, see Color Temperature. This does not require soldering but does have limits.
3. Reduce average power by controlling ON time (duty cycle). This requires bulk capacitors. Cannot lower LED voltage to save more power, must use 5V.

You can further lower power consumption by lowering power supply voltage. As the LED current is reduced the require forward voltage is also reduced. However this can lead to problems for color temperature. 

#### Color Temperature

As the LED current is reduced the require forward voltage is also reduced. This allows for the overall voltage of the LED power rail to be less. Down to around 3.3V. When lowering the peak power consumption this is generally provided for free. However to clarification purposes it is mentioned explictly. Note this does not apply to average power consumption.

However this can lead to problems for color temperature. This can be done two ways. The first is to change the LED driver resistor to be a reflection of the required current limit. The other is to program the software current gain to be a reflection of the required current limit. If this is not done the red LED is generally brighter than it is supposed to be.

### Color Depth

LEDs only support 11-13 bits of color depth. Multiplexing and power scaling reduce this. What is left over is available to creating different colors in panel.

For example four 16x32 P6 1:8 panels given 11 bit color depth LEDs would have a max color depth of 6 bits if the power was limited to 9.6 Watts. 

30mA * 32 columns * 16 rows / 8 scan * 4 panels * 5V = 38.4 Watts / 9.6 Watts = 4.

11 - log2(4) - log(8) = 11 - 2 - 3 = 6

Now lets consider lowering the power consumption to 6.8 Watts using the three approaches listed above. Here are the following bit depths:

1. 8mA * 32 columns * 16 rows / 8 scan * 4 panels * 3.3V = 6.8 Watts. 11 - roundup(log2(30 / 8)) - log2(8) = 6 bits
2. 30mA * .25 current gain * 32 columns * 16 rows / 8 scan * 4 panels * 3.3V = 6.4 Watts. 11 - roundup(log2(1 / .25)) - log2(8) = 6 bits
3. 30mA * 32 columns * 16 rows / 8 scan * 4 panels * 5V = 38.4 Watts. 11 - roundup(log2(38.4 / 6.8)) - log2(8) = 5 bits

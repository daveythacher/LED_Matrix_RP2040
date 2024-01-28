## S-PWM
Traditional PWM generally works off the idea there is only so much color depth possible. Multiplexing eats into this. Therefore the PWM period is fixed. We must divided this period by the multiplex to yield the real color depth. The grayscale clock is the max PWM period times refresh rate. (In non-hardware PWM panels this is the serial clock divided by the shift length.)

S-PWM allows you to avoid knowing the exact PWM period size. It allows you to hold the refresh rate by multiplexing more often. If the grayscale clock is only so fast, you can still protect the refresh rate. S-PWM also attempts to avoid flickering, however it does not do this in all cases. What it does is use the most significant bits which are brighter more than the least significant bits which are dimmfer. The dimmer bits are less noticeable in flicker. Traditional PWM weights these bits as equal.

S-PWM creates a variable PWM period by automating control signals into serial commands. It will proceed towards the max PWM period but will reset if the frame is swapped. If you have 12 bit PWM period on a 6.25MHz GCLK you would have a max PWM period of 11-bits at 3kHz regardless of frame rate. S-PWM is implemented as sub PWM periods where the larger PWM period is evenly divided across. This allows for the most significant bits to always be present in the sub PWM periods.

No matter how many sub PWM periods are used the result should be mostly free of flicker. However the finer detail is lost in the process potentially. Assuming you have 12 bit PWM period with 6.25MHz GCLK at 30 FPS then you would have 6-bits at 3kHz. Note this requires the sub PWM periods to be smaller or equal to 6-bits. There may be total of 16-bits possible, however only 6-bits will be used. This configuration would be 6+10. Which means there 1024 6-bit PWM periods.

If the frame rate drops then you will increase the number of effective PWM bits per frame at the given refresh rate. This is because more sub PWM periods are allowed to execute. It should also be noted that S-PWM uses a very high multiplexing rate, which has an impact on ghosting. It is preferred to match the desired sub PWM period size to the worst case effective PWM bits per frame. Otherwise the multiplexing rate will be too high. This will have an effect on ghosting. It may also not be possible in all cases.

S-PWM works on GEN 2 panels, hardware PWM panels without SRAM. GEN 3 panels, hardware PWM panels with SRAM, are subject to issues with frame rate while GEN 2 generally force this to be constant.

## Ghosting
#### Low side only algorithm
Overview is to use small pull up resistors on the columns to allow a small amount of current to trickle to the low side. This is required to prevent a LED from becoming forward biased. No current is required to flow through the LEDs to become reverse biased. This appproach can be faster but can be more expensive. Small bleeding is still possible with this approach. This happens due to the previous row's high side capacitor discharging through the columns. (A modified version of this is possible which manages this. See below.) Specialized low side drivers exist which may reduce cost.

Start by disabling the rows and the columns. (Columns can go high impedance when disabling with output enable.)

Enable the pull ups on the columns to start trickle charging the low side capacitors. 

Shift a value which will turn off columns in shift register.

Select the correct row.

Wait for the trickle charge to complete. 

Disconnect the pull ups. 

Enable the rows and columns.

Shift in the next bit plane in columns via shift register.

This is the algorithm currently used by all matrix algorithms. For GEN 1 panels turning off the columns is handle before hand. This does not anything productive without the required hardware.

##### Low side only algorithm (Modified version 1)
Overview to prevent bleeding we can make the decoder go into high impedance. Using pull downs the PMOS gate to disable the MOSFET. While in this state we choose a bit plane which enabled one or more columns. This will allow the columns for that row to drain the high side capacitor, which would prevent the bleeding in the next row. The problem is this is forbidden by many panels as the OE is not connected to the decoder.

##### Low side only algorithm (Modified version 2)
Overview use a pull down on the high side. Blank the panel as normal with the low side only algorithm. However hold the panel on the next row for a long period of time. This will allow the low side to charge and the high side to discharge. This is explained [here](https://forums.adafruit.com/viewtopic.php?t=47243). Many panels come with 1206/0805/0603 SMT resistor footprints which allow you to make these changes. However this requires specific changes to the software, see [issue](https://github.com/daveythacher/LED_Matrix_RP2040/issues/30).

#### High side only algorithm
Overview is to use small pull up resistors on the rows to allow a small amount of current to trickle to the low side. This is required since the current will flow through LED while forward biased. This approach is slower but can be cheaper. This requires a decent amount of time to multiplex. Specialized high side drivers exist which may reduce cost.

Start by disabling the rows and the columns. (Columns can go high impedance when disabling with output enable.)

Be sure to drain the PMOS gate to turn the MOSFET off. (Use push-pull decoder IC for rows, do not use high impedance when disabling with output enable.)

Enable the pull ups on the rows to start trickle charging the high and low side capacitors. 

Shift a value which will turn off columns in shift register.

Wait for the trickle charge to complete. 

Disconnect the pull ups. 

Enable the pull downs on the rows to drain the high side capacitors. (This will put the LEDs into reverse bias.) 

Wait for the draining to complete. 

Disable the pull downs. 

Select the correct row.

Release PMOS gate from drain.

Enable the rows and columns.

Shift in the next bit plane in columns via shift register.

#### High and low side algorithm
Overview this works like the high side only algorithm, but is faster. Drain the high side while charging the low side. This is approach is probably the fastest and can be the most expensive. Specialized high side and low side drivers exist which may reduce cost.

Start by disabling the rows and the columns. (Columns can go high impedance when disabling with output enable.)

Be sure to drain the PMOS gate to turn the MOSFET off. (Use push-pull decoder IC for rows, do not use high impedance when disabling with output enable.)

Enable the pull ups on the rows to start trickle charging the low side capacitors. 

Enable the pull downs on the rows to drain the high side capacitors. (This will put the LEDs into reverse bias.) 

Shift a value which will turn off columns in shift register.

Wait for the trickle charge to complete. 

Wait for the draining to complete. 

Disconnect the pull ups. 

Disable the pull downs. 

Select the correct row.

Release PMOS gate from drain.

Enable the rows and columns.

Shift in the next bit plane in columns via shift register.

## Mappers
### Color Temperature (Gamma)
As the LED current is reduced the required forward voltage is also reduced. This allows for the overall voltage of the LED power rail to be less. Down to around 3.3V. When lowering the peak power consumption this is generally provided for free. You must be careful doing this as some panels use series resistors to bleed voltage/heat.

Messing this up can lead to issues for color temperature of white. The color may be too warm or too cool. Current balancing to the get the color temperature may be required. Note some LEDs are different. There can also be differences in performance between LED colors. Manipulate the current for the peak (full) current to get the color temperature correct. Then adjust the color intensity using gamma curves per color or dot correction.

## PoE Display
16x128 (later revised to 16x96) using MBI5124 drivers with about 3-4 bits PWM at 1-3kHz refresh. This should allow decent colors and messaging flexibility. Thirty frames per second will be supported by the main controller, which should enable animations to work smoothly.

LEDs look to be capable of 12 bit, however to make it work with PoE I had to reduce this by a factor of four. The panels are eight scan panels, which lowers this by a factor of eight. Factoring in viewing distance of twelve feet this lowers it by a factor of sixteen. Factoring in dot correction this increases it by a factor of two. The resulting PWM bit count is four and these are all pretty much required by dot correction.

MBI5124 supports low side anti-ghosting and supports low current stability which is important for this PoE display. High refresh rate is used to avoid flicker since this ON/OFF PWM. A 3.3V supply rail is used to save power, allowing this to work with within the PoE powered device limit of 12.95W as defined by 802.3af.

The display is formed using four 16x32 RGB LED panels. Each panel will have its own RP2040 running this code base. These panels are standard LED drivers (GEN 1 drivers) outside of the features provided by MBI5124 for refresh and low power. There is likely no benefit in this case for hardware PWM (GEN 2 drivers) or hardware PWM with SRAM (GEN 3 drivers). Panels based on those drivers should also work, however GEN 1 drivers using multiple chains is likely good enough. GEN 3 may support using fewer chains, however finding panels using GEN 3 drivers is harder at this density and cost.

Ideally to support graphics with this display, the display would use GEN 2 drivers on custom 16x16 panels.

## Power consumption

LEDs generally support 11-13 bits of current division. This is the max color depth possible. Multiplexing increases current division which lowers color depth. The same is true for power scaling to support PoE and/or battery applications.

### Ways to lower power consumption

1. Reduce peak power by lowering the LED current via LED driver resistor. Can lower LED voltage to save more power, see Color Temperature. This does require soldering and requires a little bit of knowledge about datasheets, circuits and diodes.
2. Reduce peak power by lowering LED current via software current gain of LED driver resistor. Can lower LED voltage to save more power, see Color Temperature. This does not require soldering but does have limits. (Works better if this is implemented using analog instead of PWM.)
3. Reduce average power by controlling ON time (duty cycle). This requires bulk capacitors and probably an inrush limiter. Cannot lower LED voltage to save more power, must use 5V. Not recommended.
4. Reduce average/peak power by controlling the number of LEDs on at any given time. This gets tricky and is probably not recommended. There are two ways to do this: time sharing panels and modifying bitplanes.
5. Reduce average/peak power with current limiter. This is advanced and will not work on certain LED drivers. Not recommended.

You can further lower power consumption by lowering power supply voltage. As the LED current is reduced the require forward voltage is also reduced. However this can lead to problems for color temperature. Note some LED drivers have min constant current limits.

### Estimate power consumption
Some panels use more or less current per pixel: 30mA * columns * rows / scan * rail_voltage * panels = power consumption.

## Wiring and Signal issues
This a large subject. (TODO)

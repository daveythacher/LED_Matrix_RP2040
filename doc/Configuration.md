# Configuration
## DEFINE_APP
This is a string for the corresponding serial algorithm. Currently this is just uart.

## DEFINE_SERIAL_RGB_TYPE
This will change the number of bits per color used in the serial algorithm. Use either RGB24, RGB48, RGB_222 or RGB_555 only. Technically optional will default to RGB24.

## DEFINE_SERIAL_UART_BAUD
This is the baud rate used for the uart.

## DEFINE_ALGORITHM
This is the name of the LED panel driver or algorithm used to talk to the panel. Note some drivers from Macroblock, ChipOne, etc. are not fully documented and are suspected of having a NDA. This project does not plan to use any information violating such agreeements. (Note the modular nature of this could allow dissemination without such information.)

GEN 1 panels (standard panels) currently use ON/OFF (ON_OFF), BCM or PWM. (Traditional PWM works better with FPGA, but this is not supported by this code base.) These generally require a tradeoff in quality, refresh and/or density.

GEN 2 panels (hardware PWM panels without memory) can only be TLC5946 (incomplete) currently. These allow higher refresh rates at high quality.

GEN 3 panels (hardware PWM panels with memory) can only be TLC5958 (incomplete) currently. These allow higher densities (larger panels) at high refresh and quality.

## DEFINE_MULTIPLEX
This is the scan number marked on the back of the panel. This number is usually in the middle near a S prefix.

## DEFINE_MULTIPLEX_NAME
This is the name for multiplexing approach used. Currently available are Direct and Decoder. Direct is more common in low multiplex.

## DEFINE_MAX_RGB_LED_STEPS
This is the number of uA's supported by the LEDs without multiplexing. (This is generally something along order of 2000-8000.) Assuming the LED is capable of lighting up slightly at 2uA and the min constant forward current of the red, green and blue colors is 8mA. You should have 4000 steps or support around 12 bits of PWM if the panel was single scan.(8mA / 2uA = 4000) This is believed to be the contrast ratio of the LEDs, which determines aspects of the quality/dynamic range. (Mapping function/table and dot correct are important for ensuring color accuracy.)

The library will determine the max number of PWM bits from this number. By dividing this number by the multiplex and taking the log2 of the result. Note if you lower the forward current you should change this value to avoid wasting serial bandwidth and memory. The compiler will check for errors if this is set to an unsupported value. There is only so much memory on the RP2040, so lowering this may be required. This lowers the color depth on the device. Note this number should be whole numbers only.

## DEFINE_MAX_REFRESH
This number is the refresh rate in Hz of the panels multiplexing. When using BCM this will override the FPS. Note this number should be whole numbers only.

## DEFINE_COLUMNS
This is the number of real columns in the panel. Not the number of columns you see in the panel. If you have 16x32 with 4 scan panel you will need to set this to 64. panel_rows / (2 * scan) * panel_columns. Mapping of pixel location is not handled by the RP2040, this should be done in application logic or by logic driving serial bus. Note this number should be whole numbers only.

## DEFINE_SERIAL_CLOCK
This is the target serial bandwidth, in MHz. This is used by the compiler to verify the timing. This should not exceed 25MHz for most panels. Note you may wish to lower this is in some cases to meet timing and/or promote signal stability. (Measure rise/fall time, hold time, etc.) Note this number can have decimals.

## DEFINE_BLANK_TIME
This is the number of uS the LEDs will be off during multplexing to prevent ghosting. This is usually 1-4uS. Note this number should be whole numbers only.

## DEFINE_FPS
This is the number of FPS desired by GEN 3 panels. This is used to verify the serial clock requirements.

## DEFINE_GCLK
This is the target grayscale clock in MHz. This is used by the compiler to verify the timing. This should not exceed 33MHz for most panels. Note you may wish to lower this is in some cases to meet timing and/or promote signal stability. (Measure rise/fall time, hold time, etc.) Note this number can have decimals.

## DEFINE_RED_GAIN
Constant current gain value for Red LED. This value is constant multiple of the default gain. 1.0 configures the gain to be the default gain. Note this number can have decimals. (Only used/needed by TLC5958.)

## DEFINE_GREEN_GAIN
Constant current gain value for Green LED. This value is constant multiple of the default gain. 1.0 configures the gain to be the default gain. Note this number can have decimals. (Only used/needed by TLC5958.)

## DEFINE_BLUE_GAIN
Constant current gain value for Blue LED. This value is constant multiple of the default gain. 1.0 configures the gain to be the default gain. Note this number can have decimals. (Only used/needed by TLC5958.)

## DEFINE_IS_RAW
Boolean flag for qualifying the type of data passed over the serial algorithm. Note serial algorithm must use RGB-24. Note undefined results can occur if there is no reliable communication in serial algorithm implementation.

## DEFINE_GCLK
This is the target grayscale clock in MHz. This is used by the compiler to verify the timing. This should not exceed 33MHz for most panels. Note you may wish to lower this is in some cases to meet timing and/or promote signal stability. (Measure rise/fall time, hold time, etc.) Note this number can have decimals.


# Timing Formulas:

## Timing algorithm for BCM: (Note blanking time may reduce refresh.)

serial_clock / (multiplex * columns * max_refresh * 2^round(log2(max_rgb_led_steps / multiplex))) >= 1.0

## Timing algorithm for PWM: (Note blanking time may reduce refresh.)

serial_clock / (multiplex * columns * max_refresh * 2^(round(log2(max_rgb_led_steps / multiplex)) + 1)) >= 1.0

## Timing algorithm for ON_OFF: (Note blanking time may reduce refresh.)

serial_clock / (multiplex * columns * max_refresh * 2) >= 1.0

## Timing algorithm for TLC5946: - WIP  (Note blanking time may reduce refresh.)

serial_clock / (multiplex * columns * max_refresh * 12) >= 1.0

gclk / (multiplex * max_refresh * 2^round(log2(max_rgb_led_steps / multiplex))) >= 1.0

## Timing algorithm for TLC5958: - WIP (Note blanking time may reduce refresh.)

serial_clock / (multiplex * columns * fps * 16 * 3) >= 1.0

gclk / (multiplex * max_refresh * 2^max(round(log2(max_rgb_led_steps / multiplex)), seg_bits)) >= 1.0

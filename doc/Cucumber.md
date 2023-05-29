# Cucumber
Below is an example feature for building a custom firmware. 
```Gherkin
# features/PoE.feature

Feature: PoE Binary(s)

  Rule: Build PoE Binary(s)
  
    Example: PoE Binary with TLC5958
        Given binary named POEv2
        When using serial algorithm uart
        When using multiplexer algorithm Decoder
        When using matrix algorithm TLC5958
        And multiplex ratio of 1 : 8
        And serial clock 15 MHz
        And frame rate of 30
        And LED contrast ratio of 1 : 500
        And panel refresh rate of 100 Hz
        And panel with 128 columns
        And anti-ghosting blank time of 10 uS
        And red gain of 0.5
        And green gain of 0.75
        And blue gain of 1.0
        And grayscale clock of 17.5 MHz
        And UART baud of 7800000
        And serial protocol uses RGB-24
        Then build it
```

## Overview
The build process used here is a little messy however to simple panel configuration Cucumber is used to hide. What this does behind the scenes is build a custom xml configuration file for every Gherkin example. It will then call the Groovy build script which will then create a custom system script for calling CMake. This system script and CMake will pass the configuration parameters in the C/C++ source code for compilation. This process is used to enable multiple build flavors.

Currently the C/C++ logic requires all possible configuration parameters to be defined. CMake simply passes these through from the system script. The Groovy build script will hide parameters which are not required. Cucumber however has default settings for all required parameters which may be desired in some cases.

### Features, Rules and Examples/Scenarios
In the folder features one can add one or more features which define different build configurations. Cucumber uses Gherkin which is a BDD test framework which allows for a number of different types of input formats. It can report the results in a number of different formats and is supported by a number of different frameworks like Jenkins. You can run one feature at a time or all of them. Multiple examples/scenarios and/or rules can exist within a feature. These will always run when the feature is run.

For the time being these can be just copied. The text the follows the colon can be anything you want.

### Given
Currently there is only one of these supported. These are initial premises and are always required.

#### Binary name
It must be of the format: "Given binary named {blah}", where {blah} is a single word.

### When
One of these must be provided, but you can use as many as you want. Currently there is three of them. These are conditional premises.

#### Serial Algorithm
It must be of the format: "When using serial algorithm {blah}", where {blah} is a single word. This word is the name of a serial algorithm under LED_Matrix/lib/Serial/ with a name like serial_{blah}. Do not include the prefix "serial_". See LED_Matrix/lib/README.md for details.

If not used this will default to uart.

#### Matrix Algorithm
It must be of the format: "When using matrix algorithm {blah}", where {blah} is a single word. This word is the name of a matrix algorithm under LED_Matrix/lib/Matrix/. Type the name of the folder exactly as shown. See LED_Matrix/lib/README.md for details.

If not used this will default to BCM.

#### Multiplexer Algorithm
It must be of the format: "When using multiplexer algorithm {blah}", where {blah} is a single word. This word is the name of a multiplexer algorithm under LED_Matrix/lib/Multiplex/. Type the name of the folder exactly as shown. See LED_Matrix/lib/README.md for details.

If not used this will default to Decoder.

### And
None of these must be provided, but you can use as many as you want. These clarify conditional premises.

#### Multiplex Ratio
It must be of the format: "And multiplex ratio of 1 : {num}", where {num} is a whole integer. This is the scan of the panel which can usually be found on the back with some number like -8S-. Which would mean a 1 : 8 multiplex ratio. 

If not used this will default to 8.

#### Serial Clock
TODO

If not used this will default to 15.0.

#### Frame Rate
TODO

If not used this will default to 30.

#### LED Contrast Ratio
It must be of the format: "And LED contrast ratio of 1 : {num}", where {num} is a whole integer. This is the number of intensities which is supported by the individual LEDs. Note this is reduced by the multiplex ratio. This number is used to determine the number of PWM bits required, if supported by matrix algorithm. Many LEDs are capable of 4000 to 8000, however you may use less if desired. 

For example if a LED has a contrast of 4000 on a 1 : 8 panel, this should be set to 500. This would give you around 9 bits of color depth per color. If you wanted to use less say 6 bits, then you would set this number to 64.

If not used this will default to 500.

#### Refresh Rate
It must be of the format: "And panel refresh rate of {num} Hz", where {num} is a whole integer. This is the refresh rate in Hz which firmware will target. It could be slight above or below, but the compiler will attempt to get near this. It is recommended to use at least 100, however for some people a value of 1000 to 3000 may be recommended.

If not used this will default to 100.

#### Columns
It must be of the format: "And panel with {num} columns", where {num} is a whole integer. This is the true shift length of the panel which is not always the number of columns in the panels. It is the number of rows in the panel divided by 2, divided by the Multiplex Ratio, times the number of columns in the panel. A multiplex mapper is required to correctly address the index of the pixels, however this is handled by the application logic which generates messages over the serial algorithm.

If not used this will default to 16.

#### Anti-ghosting Timeout
It must be of the format: "And anti-ghosting blank time of {num} uS", where {num} is a whole integer. This number is used to fight small charges which build up in the panel. High refresh rates will require larger numbers, some panels handle this better than others. Some panels are not capable of high refresh rates without special logic on the high side of the panel and/or on the low side of the panel. Note large values will cause the refresh rate to be slightly lower than configured. You can compensate for this by raising the refresh rate and measurement, if required. (I just add 30-40 percent but your mileage may vary.)

If not used this will default to 1.

#### Red Current Gain
It must be of the format: "And red gain of {float}", where {float} is an integer which may include a decimal. This number is used to control the red led's intensity in software, however this is not support on all panels. It is also not supported by all matrix algorithms.

If not used this will default to 1.0.

#### Green Current Gain
It must be of the format: "And green gain of {float}", where {float} is an integer which may include a decimal. This number is used to control the green led's intensity in software, however this is not support on all panels. It is also not supported by all matrix algorithms.

If not used this will default to 1.0.


#### Blue Current Gain
It must be of the format: "And blue gain of {float}", where {float} is an integer which may include a decimal. This number is used to control the blue led's intensity in software, however this is not support on all panels. It is also not supported by all matrix algorithms.

If not used this will default to 1.0.

#### Grayscale Clock
TODO

If not given this will default to 17.0.

#### UART Baud
TODO

If not given this will default to 4000000.

#### Serial Algorithm Data format
TODO

This is useful for enabling more pixels on some panels.

If not used this will will default to 24.

### Then
Currently there is only one of these supported. These are verification premises and are always required.

#### Build Binary
It must be of the format: "Then build it". If there are no errors this will complete successfully and the output location will be printed in the log. It will be named led_{blah}, where {blah} is the word used in the Given statement.

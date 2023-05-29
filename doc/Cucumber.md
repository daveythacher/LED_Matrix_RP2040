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
Currently there is only one of these supported. These are initial premises and is always required.

#### Binary name
It must be of the format: "Given binary named {blah}", where {blah} is a single word.

### When
One of these must be provided, but you can use as many as you want. Currently there is three of them.

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
None of these must be provided, but you can use as many as you want.

#### Multiplex Ratio
It must be of the format: "And multiplex ratio of 1 : {num}", where {num} is an integer. This is the scan of the panel which can usually be found on the back with some number like -8S-. Which would mean a 1 : 8 multiplex ratio. 

If not used this will default to 8.

#### Columns
If must be of the format: "And panel with {num} columns", where {num} is an integer. This is the true shift length of the panel which is not always the number of columns in the panels. It is the number of rows in the panel divided by 2, divided by the Multiplex Ratio, times the number of columns in the panel. A multiplex mapper is required to correctly address the index of the pixels, however this is handled by the application logic which generates messages over the serial algorithm.

If not used this will 16.

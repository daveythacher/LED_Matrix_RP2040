## Matrix folder
This contains the implementation for the various Matrix algorithms. This is divided into two parts matrix and worker. Matrix is responsible for updating the display while worker is responsible for converting pixel information into the format used by the display. Note worker is not responsible for all aspects of conversion such as pixel mapping, dot correction, gamma, brightness, etc. These are handled by the host application which sends pixel information to worker via one of the Serial algorithms.

### BCM
This uses binary coded modulation (BCM) to lower them memory usage required. This is not recommended for high refresh applications.

This only supports 1st generation LED drivers.

### PWM
This uses standard PWM to create colors, however is very memory and compute intensive. This is not recommended for large displays unless willing to lower frame rate significantly. This is recommended for high refresh rate applications. 

This only supports 1st generation LED drivers. Note to use high refresh rates certain components or LED panel configurations are recommended. (Need upper and/or lower anti-ghosting functions.)

## Multiplex folder
This contains the implementations of various Multiplex algorithms. These are used by the LED panels to address individual rows.

### Direct
This works for direct address selection. Up to 5 pins may be used for a max of 1:5 multiplexing.

### Decoder
This works of multiplexers like 74HC138. Up to 5 pins may be used for a max of 1:32 multiplexing.

## pico-sdk
This is a git submodule used to interface with RP2040 hardware. Currently targets version 1.5.0.

## Serial folder
This contains the implementations for the Serial algorithms. These are divided into several parts: isr, serial and protocol specific implementations. isr is responsible for managing the allocation of the serial and matrix algorithm interrupts. Also implements required stubs for main. Serial is responsible for converting the protocol specific implementation into the serial interface used by main. Protocol specific implementations is any additional logic requried for implementation of serial algorithms.

### Serial UART
This is experimental work in progress serial algorithms for using UART, RS-232, RS-485, etc. in full duplex manner. Payloads are fixed in size no real error protocol exists. Little to no control header is used, this block style stream which uses sequential transfers rather than random operations to save computational resources.
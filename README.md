# LED_Matrix_RP2040
This code base builds a hardware module for abstracting multiplexing of LED matrix. The implementation is a memory mapped controller based on RP2040. 24-bit RGB data is sent over an IO bus to the RP2040 which is converted into the correct format for the LED display. This data is then replayed to the LED matrix repeatedly. The purpose of this project is to enable high efficiency, high determinism  and low cost for maximum refresh, density and/or color depth potential. These are enabled by the RP2040's unique hardware configuration.

Pixel mapping, brightness, color temperature/gamma, dot correction, scaling, etc. is handled by the application which produces 24-bit RGB data over the IO bus. Note IO bus may increase latency of the display. The IO bus will likely limit the size and frame rate of the display. The RP2040 has a decent amount of resources however performance tradeoffs are expected due to processing power, IO bus bandwidth/latency, memory, and HUB75 serial bandwidth. This project was originally created for a small PoE display. It is in no way capable of competing against a video wall and represents a smaller version of receiver card, in theory.

Static configuration is used but a configuration file exists which makes adjusting this fairly straightforward. Below there are some timing equations to help guide the configuration. Compile time checks will attempt to verify this configuration also. Note you are still required to perform required testing as these only help with certain issues.

There is no intention of ever supporting anything but the RP2040 or derivatives of the RP2040. Emphasis is on the RP2040 alone, so no FPGA coprocessors. Design for the project to be a hardware module/library. Standalone operation is possible, but this is outside the scope of this project. Interfacing will be kept flexible to allow a wide range of integrations.

## Status
This code base is a work in progress. Some sections are still experimental. See [this](https://github.com/daveythacher/LED_Matrix_RP2040/blob/main/LED_Matrix/README.md#status) for more details.

I am not responsible in any way for any damages or issues created in any way as a result of this code base. This code base is [licensed under GPL 3](https://github.com/daveythacher/LED_Matrix_RP2040/blob/main/LICENSE) and the terms of that are expected to be upheld. Credit where credit is due is also required.

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

## Application Information
See [this](https://github.com/daveythacher/LED_Matrix_RP2040/blob/main/Application_Infomation.md) for more details.

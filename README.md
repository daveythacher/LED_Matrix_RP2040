# LED_Matrix_RP2040
This code base builds a hardware module for abstracting multiplexing of LED matrix. The implementation is a memory mapped controller based on RP2040. RGB data is sent over an IO bus to the RP2040 which is converted into the correct format for the LED display. This data is then replayed to the LED matrix repeatedly. The purpose of this project is to enable better refresh, density and/or color depth potential at lower price point. These are enabled by the RP2040's unique hardware configuration.

Pixel mapping, brightness, color temperature/gamma, dot correction, scaling, etc. is handled by the application which produces RGB data over the IO bus. Note IO bus may increase latency of the display. This project was originally created for a small PoE display. It is in no way capable of competing against a video wall and represents a smaller version of receiver card, in theory.

Static configuration is used but a configuration file exists which makes adjusting this fairly straightforward. Note you are still required to perform required testing as these only help with certain issues.

## Status
This code base is a work in progress. Some sections are still experimental. See [this](https://github.com/daveythacher/LED_Matrix_RP2040/blob/main/LED_Matrix/README.md#status) for more details.

I am not responsible in any way for any damages or issues created in any way as a result of this code base. This code base is [licensed under GPL 3](https://github.com/daveythacher/LED_Matrix_RP2040/blob/main/LICENSE) and the terms of that are expected to be upheld. Credit where credit is due is also required.

## Contributing, feedback, questions, etc.
If in doubt use the discussion feature. I am okay with meaningful discussion in issues however please use the discussion feature first.

Some weird conventions are used. These should be documented in LED_Matrix/README.md. This is the place for documenting technical interworkings and troubleshooting. (Outside of comments.)

## Building
See [this](https://github.com/daveythacher/LED_Matrix_RP2040/blob/main/doc/Building.md) for more details.

## Configuration
### Using Cucumber/Gherkin
See [this](https://github.com/daveythacher/LED_Matrix_RP2040/blob/main/doc/Cucumber.md) for more details. For adding or disabling flavors, see features/PoE.feature.

### Manual configuration using XML
See [this](https://github.com/daveythacher/LED_Matrix_RP2040/blob/main/doc/Configuration.md) for more details. For adding or disabling flavors, see cfg.xml. 

## Application Information
See [this](https://github.com/daveythacher/LED_Matrix_RP2040/blob/main/doc/Application_Infomation.md) for more details about color depth, size, refresh rate, gamma, color temperature, dot correction, etc.

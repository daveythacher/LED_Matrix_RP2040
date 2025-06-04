# LED_Matrix_RP2040
This code base builds a hardware module for abstracting multiplexing of LED matrix. RGB data is sent over an IO bus to the RP2040. This data is then replayed to the LED matrix repeatedly.

Pixel mapping, brightness, color temperature/gamma, dot correction, scaling, etc. is handled by the application which produces RGB data over the IO bus. IO bus requires external control signals to sync, due bandwidth limitation.

## Status
This code base is a work in progress. Some sections are still experimental. See [this](https://github.com/daveythacher/LED_Matrix_RP2040/blob/main/LED_Matrix/README.md#status) for more details.

I am not responsible in any way for any damages or issues created in any way as a result of this code base. This code base is [licensed under GPL 3](https://github.com/daveythacher/LED_Matrix_RP2040/blob/main/LICENSE) and the terms of that are expected to be upheld. Credit where credit is due is also required.

## Contributing, feedback, questions, etc.
Please use the discussion feature first.

## Building
TODO

## Application Information
See [this](https://github.com/daveythacher/LED_Matrix_RP2040/blob/main/doc/Application_Infomation.md) for more details about color depth, size, refresh rate, gamma, color temperature, dot correction, etc.

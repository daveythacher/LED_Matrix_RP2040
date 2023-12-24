# Building
## Building firmware:
### Building with CMake
```bash
mkdir LED_Matrix/build
cd LED_Matrix/build
cmake .. -DPICO_SDK_PATH=../lib/pico-sdk
make -j 16
```

### Configuring with CMake
See [this](https://github.com/daveythacher/LED_Matrix_RP2040/blob/main/LED_Matrix/CMakeLists.txt) and [this](https://github.com/daveythacher/LED_Matrix_RP2040/blob/main/doc/Configuration.md) for details and more options:
```bash
mkdir LED_Matrix/build
cd LED_Matrix/build
cmake .. -DPICO_SDK_PATH=../lib/pico-sdk -DDEFINE_SERIAL_ALGORITHM=test -DDEFINE_MATRIX_ALGORITM=BCM -DDEFINE_MULTIPLEX_ALGORITHM=Direct -DDEFINE_MULTIPLEX_SCAN=4 -DDEFINE_COLUMNS=16 -DDEFINE_MAX_RGB_LED_STEPS=512 -DDEFINE_MATRIX_DCLOCK=21.0 -DDEFINE_BLANK_TIME=6 -DDEFINE_MIN_REFRESH=1500
make -j 16
```

## Building documentation:
For generating doxygen documentation:
```bash
doxygen Doxyfile
```

## Configurations:
```bash
# P6 - PoE (Power reduced by four times)
cmake .. -DPICO_SDK_PATH=../lib/pico-sdk -DDEFINE_APP=P6_PoE -DDEFINE_SERIAL_ALGORITHM=uart -DDEFINE_MULTIPLEX_ALGORITHM=Decoder -DDEFINE_MATRIX_ALGORITM=S_BCM -DDEFINE_SERIAL_RGB_TYPE=RGB24 -DDEFINE_MULTIPLEX_SCAN=8 -DDEFINE_COLUMNS=32 -DDEFINE_MAX_RGB_LED_STEPS=2048 -DDEFINE_S_PWM_SEG=4 -DDEFINE_MATRIX_DCLOCK=17.0 -DDEFINE_SERIAL_UART_BAUD=7800000 -DDEFINE_BLANK_TIME=6 -DDEFINE_FPS=30 -DDEFINE_MIN_REFRESH=3000
```
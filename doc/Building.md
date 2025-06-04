# Building
## Setup
### Installing prereqs
See scripts/Linux
### Installing submodules
See LED_Matrix/lib
## Building firmware:
### Building with CMake
```bash
mkdir LED_Matrix/build
cd LED_Matrix/build
cmake .. -DPICO_SDK_PATH=../lib/external/pico-sdk
make -j 16
```

## Building documentation:
For generating doxygen documentation:
```bash
doxygen Doxyfile
```
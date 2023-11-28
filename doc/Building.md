# Building
Currently only Linux is supported. However groovy script and CMake should allow others. (Some experimental Windows support exists.) 

## Building firmware:
### Building with CMake
TODO

### Building with Cucumber/Gherkin
Build single feature: 
```bash
cucumber features/PoE.feature
```
Each feature file should contain only one build.

### Building Binary with manual configuration of groovy script
```bash
groovy build.groovy -c cfg.xml -r
```
There should only be one enabled build in the configuration file. Only the first enabled build is actually built.

### Building Binary with manual configuration of groovy script from scratch
```bash
groovy build.groovy -c cfg.xml -r -s
```
There should only be one enabled build in the configuration file. Only the first enabled build is actually built.

## Building documentation:
For generating doxygen documentation:
```bash
doxygen Doxyfile
```
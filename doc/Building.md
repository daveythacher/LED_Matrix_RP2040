# Building
Currently only Linux is supported. However groovy script and CMake should allow others. (Some experimental Windows support exists.) 

## Building firmware:
### Building with Cucumber/Gherkin
Build single feature:
```bash
cucumber features/PoE.feature
```
Build all features in features folder:
```bash
cucumber
```
### Building Binary with manual configuration of groovy script
```bash
groovy build.groovy -c cfg.xml -r
```
### Building Binary with manual configuration of groovy script from scratch
```bash
groovy build.groovy -c cfg.xml -r -s
```
## Building documentation:
For generating doxygen documentation:
```bash
doxygen Doxyfile
```
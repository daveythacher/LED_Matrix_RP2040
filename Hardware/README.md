Most of these are prototypes and have defects. Use them as references for concepts only. Use at your own risk.

## Interface board
Files used are Interface.sch (Eagle schematic file) and Interface.brd (Eagle board file).

This board implements basic serial_uart or uart serial algorithm. Uses full duplex RS-485 transceiver capable of 10 Mbits per second. However RP2040 only supports up to 7.8 Mbits per second. 

Board is designed to fit to the back of panel. Differential UART is used to improve signal immunity from cross talk, single ended problems (power surges, distance, etc.), EMI, etc. However not all of this may be implemented correctly. Exact distance limits are not known, but cables are kept short enough to remove chances of this being a significant issue. Ethernet cable is uses for cost purposes and is not interoperable with Ethernet specifications.

Board has additional features like a reset button which help with reprogramming the chip. It also uses the RPI PICO's voltage regulator to power the board. Currently power is feed over Ethernet cable up to 5 volts. The load of the RP2040 is not believed to cause a significant issue. PICO voltage regulator is capable of step up and step down. Current on the Ethernet cable is kept low to avoid losses. Power and ground are kept on their own pairs intentionally, as is TX and RX.

### Known defects in Interface
In Interface.brd the differential signals are not routed correctly.

The HUB75 connector is assumed to work with LED Panels modified to work with a power rail at 3.3 volts. It is not recommended to use with 5 volt panels unless known to be supported. 

This is just a little note to self.

In the future adding support for hardware PWM drivers can be done using the BCM nibble notion. This wastes some memory but we are not worried about that. Max size for GEN 3 is 16x64 (8S) which is smaller than 64x128 (32S) potential. Max size for GEN 2 is 16x16 (8S). Max size for GEN 1: 16x8 (8S). (I am assuming 3kHz refresh as requirement.)

Add dot correction internally. For GEN 1 on RP2040 the max size of this is 32x8 with uint8_t as integer float. (Dual HUB75)

Removing calculator function. Making HUB75 PWM hard coded to 8x8 only.
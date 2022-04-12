#include "Multiplex/Decoder.h"
#include "hardware/gpio.h"

void __not_in_flash_func(Decoder::SetRow)(int row) {
    gpio_clr_mask(0x1F << 11);
    gpio_set_mask((row & 0x1F) << 11);
}


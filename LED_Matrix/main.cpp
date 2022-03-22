#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "pico/stdlib.h"
#include "config.h"

volatile uint8_t bank = 0;
volatile test *p;
volatile bool isReady = true;

int main() {
    extern void work();
    work();
}


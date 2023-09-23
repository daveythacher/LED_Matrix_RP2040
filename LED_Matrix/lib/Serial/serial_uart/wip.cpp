#include "pico/stdlib.h"
#include <stdint.h>
#include <algorithm>

// Preamble must be half of packet
struct Packet {
    uint8_t preamble[32];
    uint8_t inverse[16];
    uint32_t length;
    uint32_t command;
    uint32_t marker;
    uint32_t checksum;
};

static uint16_t index = 0;

//uint8_t iterator(Packet *p1, Packet *p2, uint16_t i);

void __attribute__ ((noinline)) other_task();
void __attribute__ ((noinline)) task();

int main(int argc, char **argv) {
    int LED_PIN = 0;

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    while (1) {
        gpio_put(LED_PIN, 1);
        task();
        gpio_put(LED_PIN, 0);
        other_task();
    }
}


constexpr uint8_t iterator(Packet *p1, Packet *p2, uint16_t i) {
    if (i < sizeof(Packet)) {
        uint8_t *ptr = (uint8_t *) p1;

        return *(ptr + i);
    }
    else {
        uint8_t *ptr = (uint8_t *) p2;
        i -= sizeof(Packet);

        return *(ptr + i);
    }
}

bool search(Packet *p1, Packet *p2) {
    int i, j;

    // Look for start of preamble
    for (i = std::max(index - 4, 0); i < std::min(index, (uint16_t) 4); i++) {
        if (iterator(p1, p2, i) == 0x19)
            break;
    }

    // Verify Preamble
    for (j = index; j < (int) (index + sizeof(Packet::preamble) - i); j++) {
        if (iterator(p1, p2, j) != 0x19)
            return false;
    }

    j += 4;

    // TODO: Verify checksum, inverse, marker

    // Stay in phase or realign phase
    index = i;

    return true;
}

bool isNewFrame(Packet *p1, Packet *p2) {
    uint8_t *ptr = (uint8_t *) p1;
    
    while (true) {
        // Jump out if preamble not found in Packet
        if (index >= sizeof(Packet)) {
            index -= sizeof(Packet);
            break;
        }

        // Spot check for preamble (fast reject/jump out)
        if (*(ptr + index) == 0x19)
            return search(p1, p2);
        else
            index += sizeof(Packet) / 4;
    }

    return false;
}

constexpr uint8_t rows = 3;
constexpr uint8_t columns = 16;
Packet array[columns][rows];
volatile bool isReady = false;

void reset_state_machine() {
    // TODO    
}

void state_machine(Packet *p) {
    // TODO
}

void flush_state_machine() {
    // TODO
}

void task() {
    static uint8_t last = rows;

    if (isReady) {
        // Handle boundary between rows
        if (isNewFrame(&array[columns - 1][last], &array[0][(last + 1) % rows])) 
            reset_state_machine();
        state_machine(&array[columns - 1][last]);

        // Advance to next row
        last = (last + 1) % rows;
        isReady = false;

        // Handle columns in row
        for (int i = 0; i < (columns - 1); i++) {
            if (isNewFrame(&array[i][last], &array[i + 1][last])) 
                reset_state_machine();

            state_machine(&array[i][last]);
        }

        flush_state_machine();
    }
}

void other_task() {
    static uint8_t num = 0;

    for (int i = 0; i < columns; i++) {
        for (int j = 0; j < (int) sizeof(Packet::preamble); j++) {
            array[i][num].preamble[j] = 0x19;
        }
    }

    num = (num + 1) % rows;
    isReady = true;
}
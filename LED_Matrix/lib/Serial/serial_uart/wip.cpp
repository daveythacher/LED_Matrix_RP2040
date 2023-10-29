#include "pico/stdlib.h"
#include <stdint.h>
#include <algorithm>

const int8_t header_checksum[1][4] = { {0} };

// Preamble must be half of packet
struct Packet {
    int8_t preamble[32];
    int8_t inverse[12];
    int8_t length[4];
    int8_t command[4];
    int8_t marker[4];
    int8_t header_checksum[4];
    int8_t payload_checksum[4];
};

static uint16_t index = 0;
constexpr uint8_t command_count = 1;
const int8_t marker[4] = { 0x11, 0x19, 0x19, -112 /*0x90*/ };
extern const int8_t header_checksum[command_count][4];

//uint8_t iterator(Packet *p1, Packet *p2, uint16_t i);

void __attribute__ ((noinline)) other_task();
void __attribute__ ((noinline)) task();

constexpr uint8_t rows = 3;
constexpr uint8_t columns = 16;
volatile bool isReady = false;

// Test misalignment (simulates errors in stream as seen by receiver)
typedef volatile Packet my_name[columns][rows];
#define MISALIGN
#ifdef MISALIGN
volatile Packet array2[columns*2][rows];
uint8_t *vector = (uint8_t *) &array2[0][0].preamble[0];
my_name *array;
#else
my_name array2;
my_name *array;
#endif

int main(int argc, char **argv) {
    int LED_PIN = 0;

#ifdef MISALIGN
    array = (my_name *) &vector[1];
#else
    array = &array2;
#endif

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_init(LED_PIN + 1);
    gpio_set_dir(LED_PIN + 1, GPIO_OUT);

    while (1) {
        gpio_put(LED_PIN, 1);
        gpio_put(LED_PIN + 1, 1);
        task();
        gpio_put(LED_PIN, 0);
        gpio_put(LED_PIN + 1, 0);
        other_task();
    }
}


constexpr int8_t iterator(volatile Packet *p1, volatile Packet *p2, uint16_t i) {
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

bool search(volatile Packet *p1, volatile Packet *p2) {
    unsigned int j;
    unsigned int k;
    uint32_t c;
    gpio_put(1, 1);

    // Verify Preamble
    for (j = index; j < (index + sizeof(Packet::preamble)); j++) {
        if (iterator(p1, p2, j) != 0x19) {
            gpio_put(1, 0);
            return false;
        }
    }
    gpio_put(1, 1);

    // Verify Inverse
    for (k = j; k < (j + sizeof(Packet::inverse)); k++) {
        if (iterator(p1, p2, k) != ~0x19) {
            gpio_put(1, 0);
            return false;
        }
    }
    j = k;
    gpio_put(1, 1);

    // TODO: Extract command and length
    c = 0;
    c %= command_count;
    j += 8;

    // Verify Marker
    for (k = j; k < (j + sizeof(Packet::marker)); k++) {
        if (iterator(p1, p2, k) != marker[k - j]) {
            gpio_put(1, 0);
            return false;
        }
    }
    j = k;
    gpio_put(1, 1);

    // Verify header checksum (hard coded by compiler for each command)
    //  Length is a hard coded by compiler (this is factored into the checksum)
    for (k = j; k < (j + sizeof(Packet::header_checksum)); k++) {
        if (iterator(p1, p2, k) != header_checksum[c][k - j]) {
            gpio_put(1, 0);
            return false;
        }
    }
    j = k;
    gpio_put(1, 1);

    // TODO: Extract payload checksum

    return true;
}

bool isNewFrame(volatile Packet *p1, volatile Packet *p2) {
    uint8_t *ptr = (uint8_t *) p1;
    
    while (true) {
        // Jump out if preamble not found in Packet
        if (index >= sizeof(Packet)) {
            index -= sizeof(Packet);
            break;
        }

        if (*(ptr + index) == 0x19 && search(p1, p2))
            return true;
        else
            index++;
    }

    return false;
}

void reset_state_machine() {
    // TODO    
}

void state_machine(volatile Packet *p) {
    // TODO
}

void flush_state_machine() {
    // TODO
}

void task() {
    static uint8_t last = rows;

    if (isReady) {
        // Handle boundary between rows
        if (isNewFrame(array[columns - 1][last], array[0][(last + 1) % rows])) 
            reset_state_machine();
        state_machine(array[columns - 1][last]);

        // Advance to next row
        last = (last + 1) % rows;
        isReady = false;

        // Handle columns in row
        for (int i = 0; i < (columns - 1); i++) {
            if (isNewFrame(array[i][last], array[i + 1][last])) 
                reset_state_machine();

            state_machine(array[i][last]);
        }

        flush_state_machine();
    }
}

void other_task() {
    static uint8_t num = 0;

    for (int i = 0; i < columns; i++) {
        for (int j = 0; j < (int) sizeof(Packet::command); j++) {
            array[i][num]->command[j] = 0;
        }

        for (int j = 0; j < (int) sizeof(Packet::length); j++) {
            array[i][num]->length[j] = (4096 >> (j * 8)) & 0xFF;
        }

        for (int j = 0; j < (int) sizeof(Packet::preamble); j++) {
            array[i][num]->preamble[j] = 0x19;
        }

        for (int j = 0; j < (int) sizeof(Packet::inverse); j++) {
            array[i][num]->inverse[j] = ~0x19;
        }

        for (int j = 0; j < (int) sizeof(Packet::header_checksum); j++) {
            array[i][num]->header_checksum[j] = header_checksum[0][j];
        }

        for (int j = 0; j < (int) sizeof(Packet::marker); j++) {
            array[i][num]->marker[j] = marker[j];
        }
    }

    num = (num + 2) % rows;
    isReady = true;
}

#include "pico/stdlib.h"
#include <stdint.h>
#include <algorithm>

const uint8_t header_checksum[1][4] = { {0} };

// Preamble must be half of packet
struct Packet {
    uint8_t preamble[32];
    uint8_t inverse[12];
    uint8_t length[4];
    uint8_t command[4];
    uint8_t marker[4];
    uint8_t header_checksum[4];
    uint8_t payload_checksum[4];
};

static uint16_t index = 0;
constexpr uint8_t command_count = 1;
const uint8_t marker[4] = { 0x11, 0x19, 0x19, 0x90 };
extern const uint8_t header_checksum[command_count][4];

//uint8_t iterator(Packet *p1, Packet *p2, uint16_t i);

void __attribute__ ((noinline)) other_task();
void __attribute__ ((noinline)) task();

constexpr uint8_t rows = 3;
constexpr uint8_t columns = 16;
volatile bool isReady = false;

// Test misalignment (simulates errors in stream as seen by receiver)
typedef Packet my_name[columns][rows];
#define MISALIGN
#ifdef MISALIGN
Packet array2[columns*2][rows];
uint8_t *vector = (uint8_t *) &array2[0][0].preamble[0];
my_name *array;
#else
my_name array2;
my_name *array;// = &array2;
#endif

int main(int argc, char **argv) {
    int LED_PIN = 0;

#ifdef MISALIGN
    array = (my_name *) &vector[15];
#else
    array = &array2;
#endif

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
    int i, j, k;
    uint32_t c;

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

    // Verify Inverse
    for (k = j; k < (int) (j + sizeof(Packet::inverse)); k++) {
        if (iterator(p1, p2, k) != ~0x19)
            return false;
    }
    j = k;

    // TODO: Extract command and length
    c %= command_count;
    j += 8;

    // Verify Marker
    for (k = j; k < (int) (j + sizeof(Packet::marker)); k++) {
        if (iterator(p1, p2, k) != marker[k - j])
            return false;
    }
    j = k;

    // Verify header checksum (hard coded by compiler for each command)
    //  Length is a hard coded by compiler (this is factored into the checksum)
    for (k = j; k < (int) (j + sizeof(Packet::header_checksum)); k++) {
        if (iterator(p1, p2, k) != header_checksum[c][k - j])
            return false;
    }
    j = k;

    // TODO: Extract payload checksum

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

        if (*(ptr + index) == 0x19)
            return search(p1, p2);
        else
            index++;
    }

    return false;
}

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

    num = (num + 1) % rows;
    isReady = true;
}
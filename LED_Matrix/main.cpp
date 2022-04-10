#include "matrix.h"
#include "serial.h"

int main() {
    matrix_start();
    serial_start();
    while (1) {
        serial_task();
    }
}


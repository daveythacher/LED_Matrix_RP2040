/* 
 * File:   main.cpp
 * Author: David Thacher
 * License: GPL 3.0
 */

extern void matrix_start();
extern void serial_start();
extern void loop();

int main() {
    matrix_start();
    serial_start();
    loop();
}


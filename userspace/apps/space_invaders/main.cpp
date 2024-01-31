#include <iostream>

#include "system.h"
#include "intc/intc.h"
#include "buttons/buttons.h"
#include "switches/switches.h"

#define EXIT_ERROR -1

int main() {
    // Print a welcome message
    std::cout << "Hello, World!" << std::endl;

    initialize();

    // call gameover/highscore screen


    // Return 0 to indicate successful execution
    return 0;
}

// initialize buttons/switches/intc
void initialize() {
    int err;

    // Initialize interrupt controller driver
    err = intc_init(SYSTEM_INTC_UIO_FILE);
    if (err) {
        std::cerr << "Error: " << "intc_init failed" << std::endl;
        exit(EXIT_ERROR);
    }

    // Initialize buttons
    err = buttons_init(SYSTEM_BUTTONS_UIO_FILE);
    if (err) {
        std::cerr << "Error: " << "buttons_init failed" << std::endl;
        exit(EXIT_ERROR);
    }

    // Initialize switches
    err = switches_init(SYSTEM_SWITCHES_UIO_FILE);
    if (err) {
        std::cerr << "Error: " << "switches_init failed" << std::endl;
        exit(EXIT_ERROR);
    }
}
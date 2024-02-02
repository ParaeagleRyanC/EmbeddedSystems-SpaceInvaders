#include <iostream>

#include "system.h"
#include "intc/intc.h"
#include "buttons/buttons.h"
#include "switches/switches.h"
#include "Graphics.h"
#include "Colors.h"
#include "Sprite.h"
#include "Sprites.h"

#define EXIT_ERROR -1

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

int main() {
    // Print a welcome message
    std::cout << "Hello, World!" << std::endl;

    initialize();

    // call gameover/highscore screen
    Graphics graphics;

    rgb_t green = Colors::GREEN;
    rgb_t white = Colors::WHITE;
    rgb_t black = Colors::BLACK;


    graphics.fillScreen(green);

    Sprites sprites;
    Sprite* sprite = sprites.getChar('A');
    graphics.drawSprite(sprite, 200, 200, 1, white, black);

    // Return 0 to indicate successful execution
    return 0;
}

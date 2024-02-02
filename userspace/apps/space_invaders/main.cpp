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


    graphics.fillScreen(black);

    Sprites sprites;
    Sprite* sprite = sprites.getChar('H');

    graphics.drawSprite(sprite, 50, 200, 1, white, black);
    graphics.drawSprite(sprite, 100, 200, 2, white, black);
    //graphics.drawSprite(sprite, 400, 200, 10, white, black);
    //graphics.drawStr("Hello World", 100, 200, 1, white);
    //graphics.drawStrCentered("Hello World", 200, 1, white);

    // Return 0 to indicate successful execution
    return 0;
}

#include "Gameplay.h"
#include "Bunkers.h"
#include "Tank.h"
#include "Aliens.h"
#include "Globals.h"
#include <iostream>
#include "buttons/buttons.h"
#include "switches/switches.h"
#include "intc/intc.h"
#include "UFO.h"
#include "Bullets.h"
#include "Bullet.h"


#define DEBOUNCED_MS 30
#define TEN_MS 10
#define EXIT_ERROR -1

static uint32_t clockTimer = 0;
static uint32_t secondsTimer = 0;
static uint32_t secondsTimerD1 = -1;
static uint8_t buttons;
static uint8_t button;
static uint16_t debounceTimer = 0;
static uint16_t fastIncDecTimer = 0;
static uint8_t nextBtnPresses = 0;
static uint16_t alienFireTimer = 0;
bool canMoveTank = false;

Gameplay::Gameplay()
{
    this->tank = new Tank();
    this->aliens = new Aliens();
    aliens->initialize();

    this->bullets = new Bullets();

    Globals::getGraphics().fillScreen(Globals::getBackgroundColor());

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

        // Enable interrupt output from buttons
    buttons_enable_interrupts();

    // Enable button and FIT interrupt lines on interrupt controller
    intc_irq_enable(SYSTEM_INTC_IRQ_BUTTONS_MASK);
    intc_irq_enable(SYSTEM_INTC_IRQ_FIT_MASK);
}

void Gameplay::drawInit()
{
    Globals::getScore().draw();
    Globals::getLives().draw();
    Globals::getBunkers().draw();
    tank->draw();
    Globals::getGraphics().drawLine(Globals::getColorGreen());
    aliens->draw();
}

// Check all collisions by calling all appropriate collision functions.  This
// only needs to be done if some game object moved during the last tick.
void Gameplay::checkCollisions() {
    Globals::getBunkers().checkCollisions();
    tank->checkCollisions();
}

void Gameplay::tick()
{
    debounceTimer += TEN_MS;

    Globals::getUFO().tick();
    Globals::getBullets().tick();
    aliens->generateRandomFireDelay();

    //this is the block for the tank
    if (debounceTimer == DEBOUNCED_MS) {
        canMoveTank = true;
        if (buttons == BUTTONS_2_MASK) button = 2;
        else if (buttons == BUTTONS_3_MASK) button = 3;
        else if (buttons == BUTTONS_1_MASK) button = 1;
    }

    // one press one move
    if (canMoveTank) {
        canMoveTank = false;
        tank->tick(button);
    }
    // long press fast move
    if (debounceTimer >= 500 && buttons) {
        fastIncDecTimer += TEN_MS;
        tank->tick(button);
        fastIncDecTimer = 0;
    }

    aliens->tick();

    checkCollisions();

}

void Gameplay::buttons_isr()
{
    buttons = buttons_read();
    debounceTimer = 0;
    buttons_ack_interrupt();
}

void Gameplay::end_game()
{
    intc_exit();
    buttons_exit();
}
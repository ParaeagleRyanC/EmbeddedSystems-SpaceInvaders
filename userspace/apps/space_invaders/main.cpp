#include <iostream>
#include <thread>
#include <chrono>

#include "system.h"
#include "intc/intc.h"
#include "buttons/buttons.h"
#include "switches/switches.h"
#include "Globals.h"
#include "Graphics.h"
#include "Colors.h"
#include "Sprite.h"
#include "Score.h"
#include "Sprites.h"
#include "HighScores.h"
#include "Tank.h"

#define EXIT_ERROR -1

// initialize buttons/switches/intc
void initialize() {

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

static uint8_t buttons;
static uint8_t button;
static uint16_t debounceTimer = 0;
static uint16_t fastIncDecTimer = 0;
bool canMoveTank = false;
#define DEBOUNCED_MS 30
#define TEN_MS 10

// This is invoked each time there is a change in the button state (result of a
// push or a bounce).
void isr_buttons() {
  buttons = buttons_read();
  debounceTimer = 0;
  buttons_ack_interrupt();
}

uint8_t nextBtnPresses = 0;

// This is invoked in response to a timer interrupt.
// help debounce buttons
void isr_fit() {
  // increment debounce timer
  debounceTimer += TEN_MS;

// this block for highscores
  // button debounced, proceed to increment/decrement
//   if (debounceTimer == DEBOUNCED_MS) {
//     if (buttons == BUTTONS_0_MASK) button = DONE_BTN;
//     else if (buttons == BUTTONS_1_MASK) {
//         button = NEXT_BTN;
//         nextBtnPresses++;
//     }
//     else if (buttons == BUTTONS_2_MASK) button = DEC_LETTER;
//     else if (buttons == BUTTONS_3_MASK) button = INC_LETTER;
//   }

  // this block for tank
    if (debounceTimer == DEBOUNCED_MS) {
        canMoveTank = true;
        if (buttons == BUTTONS_2_MASK) button = 2;
        else if (buttons == BUTTONS_3_MASK) button = 3;
    }
}

int main() {

    initialize();

    HighScores highScores = HighScores(4500);

    Tank tank = Tank();
    tank.draw();

    bool saveHighscores = true;


    Globals::getScore().draw();
    Globals::getLives().draw();

    // Sleep for 2 seconds for testing
    //std::this_thread::sleep_for(std::chrono::seconds(2));
    Globals::getLives().loseALife();

    // Sleep for 2 seconds for testing
    //std::this_thread::sleep_for(std::chrono::seconds(2));
    Globals::getLives().gainALife();

    Globals::getBunkers().draw();

    Globals::getUFO().draw();

    // graphics.drawStrCentered("GAME OVER", 15, 5, Globals::getColorWhite()); // this will be way bigger
    // graphics.drawStrCentered("ENTER YOUR NAME", 55, 2, Globals::getColorWhite()); // slightly smaller


    while (1) {
        // Call interrupt controller function to wait for interrupt
        uint32_t interrupts = intc_wait_for_interrupt();

        // Check which interrupt lines are high and call the appropriate ISR
        // functions
        if (interrupts & SYSTEM_INTC_IRQ_FIT_MASK) {
            // call isr_fit() to debounce button
            isr_fit();

            // // call tickUserEntry up to 3 next btn presses
            // if (nextBtnPresses < 3) {
            //     highScores.tickUserEntry(button);
            //     button = 0;
            // }
            // else if (saveHighscores){
            //     highScores.save();
            //     saveHighscores = false;
            // }

            // one press one move
            if (canMoveTank) {
                canMoveTank = false;
                tank.tick(button);
            }
            // long press fast move
            if (debounceTimer >= 500 && buttons) {
                fastIncDecTimer += TEN_MS;
                tank.tick(button);
                fastIncDecTimer = 0;
            }
            
        }

        if (interrupts & SYSTEM_INTC_IRQ_BUTTONS_MASK)
            isr_buttons();

        // Acknowledge the intc interrupt
        intc_ack_interrupt(interrupts);

        // Re-enable UIO interrupts
        intc_enable_uio_interrupts();
    }

    intc_exit();
    buttons_exit();

    return 0;
}

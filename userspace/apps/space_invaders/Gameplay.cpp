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
#include "HighScores.h"
#include "audio_config/audio_config.h"

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
static uint16_t gameOverSettleCounter = 0;
static bool canMoveTank = false;
static bool gameOver = false;

Gameplay::Gameplay() {
    gameVolume = 30;
    saveHighscores = true;

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

    // Initialize the audio codec chip via I2C
    audio_config_init();

    // set volume
    audio_config_set_volume(gameVolume);

    // Enable interrupt output from buttons
    buttons_enable_interrupts();

    // Enable button and FIT interrupt lines on interrupt controller
    intc_irq_enable(SYSTEM_INTC_IRQ_BUTTONS_MASK);
    intc_irq_enable(SYSTEM_INTC_IRQ_FIT_MASK);
}

void Gameplay::drawInit() {
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
    Globals::getUFO().checkCollisions();
    aliens->checkCollisions();
}

void Gameplay::tick() {
    debounceTimer += TEN_MS;

    // set volume
    if (debounceTimer == DEBOUNCED_MS) {
        if (switches_read() & SWITCHES_0_MASK) {
            if (buttons_read() & BUTTONS_3_MASK) {
                if (gameVolume < 60) gameVolume += 3;
                audio_config_set_volume(gameVolume);
            }
        }
        else {
            if (buttons_read() & BUTTONS_3_MASK) {
                if (gameVolume > 0) gameVolume -= 3;
                audio_config_set_volume(gameVolume);
            }
        }
    }
    
    
    if (gameOver) {
        Globals::getPlaySound().loopOff();

        if (debounceTimer == DEBOUNCED_MS) {
            if (buttons == BUTTONS_0_MASK) button = DONE_BTN;
            else if (buttons == BUTTONS_1_MASK) {
                button = NEXT_BTN;
                nextBtnPresses++;
            }
            else if (buttons == BUTTONS_2_MASK) button = DEC_LETTER;
            else if (buttons == BUTTONS_3_MASK) button = INC_LETTER;
        }

        // call tickUserEntry up to 3 next btn presses
        if (nextBtnPresses < 3) {
            tickHighScores();
            button = 0;
        }
        else if (saveHighscores){
            highScores->save();
            saveHighscores = false;
        }
    }

    // this is the block for the tank
    else if (debounceTimer == DEBOUNCED_MS) {
        canMoveTank = true;
        if (buttons & BUTTONS_2_MASK) button = 2;
        else if (buttons & BUTTONS_0_MASK) button = 0;
        else if (buttons & BUTTONS_1_MASK) button = 1;
        else button = 3;
    }

    else {
        Globals::getUFO().tick();
        Globals::getBullets().tick();
        aliens->generateRandomFireDelay();

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

        // tick if tank got hit
        if (tank->isTankHit()) tank->tick(0);

        if (aliens->numAlive() == 0) {
            //this->aliens = new Aliens();
            aliens->initialize();
            aliens->draw();
            Globals::getLives().gainALife();
        }

        aliens->tick();

        checkCollisions();

        // do gameover stuff
        if(Globals::getLives().isGameOver() || aliens->reachedBottom()) {
            uint8_t livesLeft = Globals::getLives().getNumLives();
            for (int i = 0; i < livesLeft; i++) Globals::getLives().loseALife();
            Globals::getGraphics().fillScreen(Globals::getBackgroundColor());
            Globals::getGraphics().drawStr("            ", 8, 5, 2, Globals::getBackgroundColor());
            Globals::getGraphics().drawStr("     ", 385, 8, 2, Globals::getBackgroundColor());
            Globals::getGraphics().drawSprite(Globals::getSprites().getUFO(), Globals::getUFO().getX(), Globals::getUFO().getY(), UFO_SIZE, Globals::getBackgroundColor());
            Globals::getGraphics().drawStrCentered("GAME OVER", 15, 5, Globals::getColorWhite()); // this will be way bigger
            Globals::getGraphics().drawStrCentered("ENTER YOUR NAME", 55, 2, Globals::getColorWhite()); // slightly smaller
            this->highScores = new HighScores(Globals::getScore().getScore());
            gameOver = true;
        }
    }
}

void Gameplay::tickHighScores() { highScores->tickUserEntry(button); }

void Gameplay::buttons_isr() {
    buttons = buttons_read();
    debounceTimer = 0;
    buttons_ack_interrupt();
}

void Gameplay::end_game() {
    intc_exit();
    buttons_exit();
}
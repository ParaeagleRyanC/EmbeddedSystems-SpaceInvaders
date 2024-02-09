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
#include "UFO.h"
#include "Gameplay.h"

#define EXIT_ERROR -1

int main() {

    HighScores highScores = HighScores(4500);
    bool saveHighscores = true;

    Gameplay game = Gameplay();
    game.drawInit();
    
    while (1) {
        uint32_t interrupts = intc_wait_for_interrupt();
        if (interrupts & SYSTEM_INTC_IRQ_FIT_MASK) {game.tick();}
        if (interrupts & SYSTEM_INTC_IRQ_BUTTONS_MASK) {game.buttons_isr();}
        intc_ack_interrupt(interrupts);
        intc_enable_uio_interrupts();
    }

    game.end_game();

    return 0;
}

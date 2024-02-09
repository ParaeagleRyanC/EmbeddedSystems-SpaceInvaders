#include "Tank.h"
#include "Globals.h"

// Player tank (initialized to starting location)
Tank::Tank() : GameObject(Globals::getSprites().getTank(SPRITE_TANK), 0, 450, 2, Globals::getColorGreen()) {
    tickCnt = 0;
    state = TANK_STATE_ALIVE;
    flagExplosion = false;
}

// Tick the tank
bool Tank::tick(uint8_t btn) {
    switch (state) {
        // move the tank
        case TANK_STATE_ALIVE:
                if (btn == MOVE_LEFT && x > 0) move(Globals::getSprites().getTank(SPRITE_TANK), -1, 0);
                else if (btn == MOVE_RIGHT && x < 599) move(Globals::getSprites().getTank(SPRITE_TANK), 1, 0);
            // if (tickCnt == tickMax) {
            //     tickCnt = 0;
            //     if (btn == MOVE_LEFT && x > 0) move(Globals::getSprites().getTank(SPRITE_TANK), -1, 0);
            //     else if (btn == MOVE_RIGHT && x < 590) move(Globals::getSprites().getTank(SPRITE_TANK), 1, 0);
            // }
            // else tickCnt++;

    }
    return true; // probably have to change this later
}

// Check for collisions between alien bullets and the tank
void Tank::checkCollisions() {
    // do something later
}

// Kill the tank.  This should flag the explosion (but don't call
// GameObject.kill(), otherwise the tank won't be drawn)
void Tank::kill() {
    // do something later
}
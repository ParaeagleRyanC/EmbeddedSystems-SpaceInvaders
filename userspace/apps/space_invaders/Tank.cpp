#include "Tank.h"
#include "Globals.h"
#include "system.h"
#include "config.h"

// Player tank (initialized to starting location)
// Tank::Tank() : GameObject(Globals::getSprites().getTank(SPRITE_TANK), 0, 450, 2, Globals::getColorGreen()) {
Tank::Tank() : GameObject(Globals::getSprites().getTank(SPRITE_TANK), 0, TANK_Y, TANK_SIZE, Globals::getColorGreen()), 
                tickCnt(0), tickMax(TANK_MOVE_DELAY_SECONDS / SYSTEM_FIT_PERIOD_SECONDS),
                deathTickMax(TANK_DEATH_DURATION_SECONDS / SYSTEM_FIT_PERIOD_SECONDS),
                state(TANK_STATE_ALIVE), flagExplosion(false) {}

// Tick the tank
bool Tank::tick(uint8_t btn) {
    //Increase tick count
    tickCnt++;

    switch (state) {
        // Tank Alive State
        case TANK_STATE_ALIVE:
            if (btn == MOVE_LEFT && x > 0 && tickCnt >= tickMax) {
                move(Globals::getSprites().getTank(SPRITE_TANK), -TANK_MOVE_X_DISTANCE, 0);
                tickCnt = 0;
            }
            else if (btn == MOVE_RIGHT && x < GRAPHICS_WIDTH - getWidth() - 1 && tickCnt >= tickMax) {
                move(Globals::getSprites().getTank(SPRITE_TANK), TANK_MOVE_X_DISTANCE, 0);
                tickCnt = 0;
            }
            else if (flagExplosion) {
                flagExplosion = false;
                move(Globals::getSprites().getTank(SPRITE_TANK_EXPLOSION1), 0, 0);
                state = TANK_STATE_DYING1;
            }
            break;
        // Tank Dying State 1
        case TANK_STATE_DYING1:
            if(tickCnt >= deathTickMax) {
                tickCnt = 0;
                move(Globals::getSprites().getTank(SPRITE_TANK_EXPLOSION2), 0, 0);
                state = TANK_STATE_DYING2;
            }
            break;
        // Tank Dying State 2
        case TANK_STATE_DYING2:
            if(tickCnt >= deathTickMax) {
                tickCnt = 0;
                state = TANK_STATE_ALIVE;
            }
            break;
        
    }
    return true; // probably have to change this later
}

// Check for collisions between alien bullets and the tank
void Tank::checkCollisions() {
    if(isOverlapping()) {
        Tank::kill();
    }
}

// Kill the tank.  This should flag the explosion (but don't call
// GameObject.kill(), otherwise the tank won't be drawn)
void Tank::kill() {
    flagExplosion = true;
}
#include "UFO.h"
#include "config.h"
#include "system.h"
#include "Globals.h"

#include <time.h>

// **Explosion testing material. Delete once tested**
uint8_t tempTickExplode = (UFO_MOVE_DELAY_SECONDS * 50 / SYSTEM_FIT_PERIOD_SECONDS);\
uint8_t tempTickCnt = 0;

// The flying UFO object
UFO::UFO() : GameObject(Globals::getSprites().getUFO(), 0, UFO_Y, UFO_SIZE, Globals::getColorRed()),
                tickCnt(0), moveTickMax(UFO_MOVE_DELAY_SECONDS / SYSTEM_FIT_PERIOD_SECONDS),
                hideTickMax(UFO::getRandomHideDelayTicks()), state(HIDDEN), flagKill(false) {}

// Calculate the random number of ticks until the UFO should appear
uint32_t UFO::getRandomHideDelayTicks() {
    srand (time(NULL));
    uint32_t numSecs = rand() % (UFO_HIDE_DELAY_MAX_SECONDS-UFO_HIDE_DELAY_MIN_SECONDS) + UFO_HIDE_DELAY_MIN_SECONDS;
    return numSecs / SYSTEM_FIT_PERIOD_SECONDS;
}

// Tick the UFO and return whether it moved
bool UFO::tick() {
    //Increase tick count
    tickCnt++;

    switch(state) {
        // Hidden State
        case HIDDEN:
            if(tickCnt >= hideTickMax) {
                // Respawn
                if(!isAlive()) resurrect();
                // Reappear
                else draw();
                tickCnt = 0;
                state = MOVING;
            }
            break;

        // Moving State
        case MOVING:
            // **Explosion testing material.  Delete once tested**
            // tempTickCnt++;
            // if(tempTickCnt >= tempTickExplode) {
            //     flagKill = true;
            // }



            // Move over more
            if(tickCnt >= moveTickMax) {
                move(Globals::getSprites().getUFO(), UFO_MOVE_X_DISTANCE, 0);
                tickCnt = 0;
                return true;
            }
            // Reach the end of the screen
            else if(getX() + getWidth() > GRAPHICS_WIDTH) {
                move(Globals::getSprites().getUFO(), -getX(), 0);
                erase();
                state = HIDDEN;
            }
            // Get destroyed
            else if(flagKill) {
                move(Globals::getSprites().getAlienExplosion(), 0, 0);
                tickCnt = 0;
                flagKill = false;
                state = EXPLODING;
            }
            break;
        // Exploding State
        case EXPLODING:
            printf("Tick Count: %d/%d\n", tickCnt, (ALIENS_MOVE_DELAY_SECONDS / SYSTEM_FIT_PERIOD_SECONDS));
            if(tickCnt >= (ALIENS_MOVE_DELAY_SECONDS / SYSTEM_FIT_PERIOD_SECONDS)) {
                // Figure out why erase isn't erasing the entire explosion mark
                erase();
                kill();
                // Figure out why it respawns in plain sight
                move(Globals::getSprites().getUFO(), -getX(), 0);
                tickCnt = 0;
                state = HIDDEN;
            }
            break;
    }
    return false;
}

// Check for collisions between the player bullet and the UFO
void UFO::checkCollisions() {
    // if(GameObject::isOverlapping()) {
    //     flagKill = true;
    // }
}
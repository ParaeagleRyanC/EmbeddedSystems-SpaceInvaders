#include "UFO.h"
#include "Sprites.h"
#include "config.h"
#include "system.h"
#include "Globals.h"

#include <time.h>

// The flying UFO object
UFO::UFO() : GameObject(Globals::getSprites().getUFO(), 0, UFO_Y, UFO_SIZE, UFO_COLOR), tickCnt(0), moveTickMax(UFO_MOVE_DELAY_SECONDS / SYSTEM_FIT_PERIOD_SECONDS),
hideTickMax(UFO::getRandomHideDelayTicks), state(HIDDEN), flagKill(false) {}


//  {
//     tickCnt = 0;
//     moveTickMax = (UFO_MOVE_DELAY_SECONDS / SYSTEM_FIT_PERIOD_SECONDS);
//     hideTickMax = getRandomHideDelayTicks();
//     state = HIDDEN;
//     flagKill = false;
// }

// Calculate the random number of ticks until the UFO should appear
uint32_t UFO::getRandomHideDelayTicks() {
    srand (time(NULL));
    uint32_t numSecs = rand() % 10 + 5;
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
                // if() resurrect();
                // else if() move(nullptr, -GRAPHICS_WIDTH, 0);
                tickCnt = 0;
                state = MOVING;
            }
            break;
        // Moving State
        case MOVING:
            // Move over more
            if(tickCnt >= moveTickMax) {
                move(nullptr, 10, 0);
                tickCnt = 0;
                return true;
            }
            // Reach the end of the screen
            else if(GameObject::getX() + GameObject::getWidth() > GRAPHICS_WIDTH) {
                GameObject::erase();
            }
            // Get destroyed
            else if(flagKill) {
                GameObject::kill();
            }
            break;
        return false;
    }
}

// Check for collisions between the player bullet and the UFO
void UFO::checkCollisions() {
    // if(GameObject::isOverlapping()) {
    //     flagKill = true;
    // }
}
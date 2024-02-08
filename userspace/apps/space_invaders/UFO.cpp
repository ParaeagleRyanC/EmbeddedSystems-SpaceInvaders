#include "UFO.h"
#include "Sprites.h"
#include "Colors.h"
#include "config.h"
#include "system.h"

#include <time.h>

// The flying UFO object
UFO::UFO() {
    tickCnt = 0;
    moveTickMax = UFO_MOVE_DELAY_SECONDS / SYSTEM_FIT_PERIOD_SECONDS;
    hideTickMax = getRandomHideDelayTicks();
    state = HIDDEN;
    flagKill = false;
    GameObject::GameObject(Sprites::getUFO, 0, UFO_Y, UFO_SIZE, UFO_COLOR);
}

// Calculate the random number of ticks until the UFO should appear
uint32_t UFO::getRandomHideDelayTicks() {
    srand (time(NULL));
    uint32_t numSecs = rand() % 10 + 5
    return numSecs / SYSTEM_FIT_PERIOD_SECONDS;
}

// Tick the UFO and return whether it moved
bool UFO::tick() {
    //Increase tick count
    tickCnt++;

    // Hidden State
    case Hidden:
        if(tickCnt >= hideTickMax) {
            if() resurrect();
            else if() move(nullptr, -GRAPHICS_WIDTH, 0);
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
        }
        // Reach the end of the screen
        else if(getX + getWidth > GRAPHICS_WIDTH) {
            erase();
        }
        // Get destroyed
        else if(flagKill) {
            kill();
        }
        break;
}

// Check for collisions between the player bullet and the UFO
void UFO::checkCollisions() {
    if(isOverlapping()) {
        flagKill = true;
    }
}
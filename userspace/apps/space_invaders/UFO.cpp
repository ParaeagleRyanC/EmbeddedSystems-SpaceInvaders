#include "UFO.h"
#include "Sprites.h"
#include "Colors.h"

#include <time.h>

// The flying UFO object
UFO::UFO() {
    tickCnt = 0;
    moveTickMax = 30;
    hideTickMax = 0;
    state = HIDDEN;
    flagKill = false;
    GameObject::GameObject(Sprites::getUFO, 0, 0, 1, Colors::RED);
}

// Calculate the random number of ticks until the UFO should appear
uint32_t getRandomHideDelayTicks() {
    srand (time(NULL));
    return rand() % 1000 + 1000
}

// Tick the UFO and return whether it moved
bool tick() {
    if
}

// Check for collisions between the player bullet and the UFO
void checkCollisions() {
    
}
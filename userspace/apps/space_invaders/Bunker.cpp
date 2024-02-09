#include "Bunker.h"
#include "Globals.h"




// Create bunker at given x,y location. 
// Make sure this calls the parent constructor.
Bunker::Bunker(uint16_t x, uint16_t y) : GameObject(Globals::getSprites().getBunker(), x, y, BUNKER_SIZE, Globals::getColorGreen()) {
    this->x = x,
    this->y = y;
    size = BUNKER_SIZE;
    // do something with the bunkerBlocks vector
}

// Check collision between both player and enemy bullets and the bunker.
bool Bunker::checkBulletCollision(Bullet *bullet) {
    // Do SOMETHING
    return false;
}
#include "Bunker.h"
#include "Globals.h"
#include "Bullet.h"




// Create bunker at given x,y location. 
// Make sure this calls the parent constructor.
Bunker::Bunker(uint16_t x, uint16_t y) : GameObject(Globals::getSprites().getBunker(), x, y, BUNKER_SIZE, Globals::getColorGreen()) {
    this->x = x,
    this->y = y;
    size = BUNKER_SIZE;

    uint16_t xPos;
    uint16_t yPos;
    for (int i = 0; i < 4; i++) {
        xPos = x + 6 * i;
        for (int j = 0; j < 3; j++) {
            yPos = y + 6 * j;
            bunkerBlocks.push_back(new BunkerBlock(xPos, yPos));
        }
        yPos = y;
    }
}

// Check collision between both player and enemy bullets and the bunker.
bool Bunker::checkBulletCollision(Bullet *bullet) {
    if (!bullet->isAlive()) return false;
    for (auto bunkerBlock : bunkerBlocks) {
        // /std::cout << "blockx: " << bunkerBlock->getX() << " blocky: " << bunkerBlock->getY() << std::endl;
        if (bunkerBlock->isAlive() && bunkerBlock->isOverlapping(bullet)) {
        //std::cout << "bulletx: " << bullet->getX() << " bullety: " << bullet->getY() << std::endl;
            bunkerBlock->inflictDamage();
            bunkerBlock->drawNoBackground();
            bullet->kill();
            return true;
        }
    }
    return false;
}
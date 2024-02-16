#include "Aliens.h"
#include "Alien.h"
#include "Globals.h"
#include "config.h"
#include "system.h"
#include "GameObject.h"

#include <time.h>

static bool goDown = false;

Aliens::Aliens() {
    moveTickCnt = 0;
    fireTickCnt = 0;
    fireTickMax = 0;
    movingLeft = false;
    numAliensAlive = 55;
    reachedBunker = false;
}

// private function
// Returns the bottom most alien that is alive in a column, 
// and nullptr if they are all dead.
Alien Aliens::*getBottomAlienInColumn(uint8_t col) {
    return NULL;
}

// Initialize all aliens.  This can be called again when all aliens are dead
// to reset the alien group.
void Aliens::initialize() {
    // create all alien objects
    uint16_t init_x = 240;
    uint16_t init_y = 80;
    sprite_alien_type_t typeIn;
    sprite_alien_type_t typeOut;
    for (int i = 0; i < 5; ++i) {
        if (i == 0) {
            typeIn = SPRITE_ALIEN_TOP_IN;
            typeOut = SPRITE_ALIEN_TOP_OUT;
        }
        else if (i == 1 || i == 2) {
            typeIn = SPRITE_ALIEN_MID_IN;
            typeOut = SPRITE_ALIEN_MID_OUT;
        }
        else {
            typeIn = SPRITE_ALIEN_BOT_IN;
            typeOut = SPRITE_ALIEN_BOT_OUT;
        }
        std::vector<Alien *> tempAlienVector;
        for (int j = 0; j < 11; ++j) {
            Alien *tempAlien = new Alien(
                Globals::getSprites().getAlien(typeIn),
                Globals::getSprites().getAlien(typeOut), init_x, init_y);
            tempAlienVector.push_back(tempAlien);
            init_x += Globals::getSprites().getAlien(typeIn)->getWidth() + 22;
        }
        this->aliens.push_back(tempAlienVector);
        init_y += Globals::getSprites().getAlien(typeIn)->getHeight() + 22;
        init_x = 240;
    }
}

// Draw all the aliens (probably only need to call this at the beginning)
void Aliens::draw() {
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 11; ++j) {
            aliens[i][j]->draw();
        }
    }
}

// check for the live alien on either side of the group
// and update moving direction when needed
void Aliens::updateMovingDirection() {
    if (movingLeft) {
        for (int i = 0; i < 11; ++i) {
            for (int j = 0; j < 5; ++j) {
                if (aliens[j][i]->isAlive() && aliens[j][i]->getX() <= 5) {
                    movingLeft = !movingLeft;
                    goDown = true;
                    return;
                }
            }
        }
    }
    else {
        for (int i = 10; i >= 0; --i) {
            for (int j = 0; j < 5; ++j) {
                if (aliens[j][i]->isAlive() && aliens[j][i]->getX() >= 610) {
                    movingLeft = !movingLeft;
                    goDown = true;
                    return;
                }
            }
        }
    }
}

// Tick the alien group, and return whether something moved.
bool Aliens::tick() {
    updateMovingDirection();
    for(int i = 0; i < 5; i++) {
        std::vector<Alien *> tempAlienRow = this->aliens.at(i);
        for (int j = 0; j < 11; j++) {
            // Draw the explosion if the alien is exploding and then set it to not exploding
            if(tempAlienRow[j]->isExploding()) {
                // Change movement numbers if you want the explosion to be in line with the aliens
                tempAlienRow[j]->move(Globals::getSprites().getAlienExplosion(), 0, 0);
                // Set exploding to false
                tempAlienRow[j]->resetExploding();
            }
            // Only draw and move the alien if it is alive
            else if(tempAlienRow[j]->isAlive()) {
                if(movingLeft){tempAlienRow[j]->moveLeft();}
                else{tempAlienRow[j]->moveRight();}
                if(goDown) {
                    tempAlienRow[j]->moveDown();
                }
            }
        }
    }
    goDown = false;
    return false;
}

// Check for collisions between player bullet and aliens, killing player
// bullet and alien when they are overlapping.
void Aliens::checkCollisions() {
    // Loop through the vector of aliens
    // for (const auto& alien : aliens) {
    for(int i = 0; i < 5; i++) {
        std::vector<Alien *> tempAlienRow = this->aliens.at(i);
        for (int j = 0; j < 11; j++) {
            // Only check collisions for alive aliens
            if (tempAlienRow[j]->isAlive() && tempAlienRow[j]->isOverlapping(Globals::getBullets().getPlayerBullet())) {
                Globals::getBullets().getPlayerBullet().kill();
                tempAlienRow[j]->explode();

                // May need to:
                // Make it so that only alive aliens are drawn
                // Make hit aliens have the explode sprite for ony one tick (check alien tick speed)
                //      explode() in alien.cpp
                // Resurrect all aliens at a new round

                // Need to implement bullets for the tank (ryan might be working on that)
                //      Cant test otherwise
                // Test tank explosions
            }
        }
    }
}

// Generate a random number of ticks for firing the next alien bullet and
// store in fireTickMax
void Aliens::generateRandomFireDelay() {
    // srand (time(NULL));
    // uint32_t numSecs = rand() % (ALIENS_BULLET_MAX_WAIT_SECONDS-ALIENS_BULLET_MIN_WAIT_SECONDS) + ALIENS_BULLET_MIN_WAIT_SECONDS;
    // fireTickMax = numSecs / SYSTEM_FIT_PERIOD_SECONDS;
}
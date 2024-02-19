#include "Aliens.h"
#include "Alien.h"
#include "Globals.h"
#include "config.h"
#include "system.h"
#include "GameObject.h"
#include "Bullet.h"

#include <time.h>
#include <random>

static bool goDown = false;
static int numAliensInCol[] = {5,5,5,5,5,5,5,5,5,5,5};
static uint16_t alienFireTimer = 0;
static uint16_t clockTimer=0;
static uint16_t secondsTimer=0;
static uint16_t secondsTimerD1=-1;

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
// not used
Alien Aliens::*getBottomAlienInColumn(uint8_t col) { return NULL; }

// Initialize all aliens.  This can be called again when all aliens are dead
// to reset the alien group.
void Aliens::initialize() {
    aliens.clear();
    moveTickCnt = 0;
    fireTickCnt = 0;
    fireTickMax = 0;
    movingLeft = false;
    numAliensAlive = 55;
    reachedBunker = false;

    // create all alien objects
    uint16_t init_x = 240;
    uint16_t init_y = 80;
    sprite_alien_type_t typeIn;
    sprite_alien_type_t typeOut;
    bool bottomFlag = false;
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
            if (i == 4) { bottomFlag = true;}
            Alien *tempAlien = new Alien(
                Globals::getSprites().getAlien(typeIn),
                Globals::getSprites().getAlien(typeOut), init_x, init_y,bottomFlag);
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
    alienFireTimer++;
    clockTimer++;
    secondsTimer = (clockTimer * 0.05);
    updateMovingDirection();
    
    if (secondsTimer > secondsTimerD1) {
        for (int i = 0; i < 5; i++) { 
            std::vector<Alien *> tempAlienRow = this->aliens.at(i);
            for (int j = 0; j < 11; j++) {
                // Draw the explosion if the alien is exploding and then set it to not exploding
                if(tempAlienRow[j]->isExploding()) {
                    // Change movement numbers if you want the explosion to be in line with the aliens
                    tempAlienRow[j]->move(Globals::getSprites().getAlienExplosion(), 0, 0);
                    // Set exploding to false
                    tempAlienRow[j]->resetExploding();
                    tempAlienRow[j]->eraseOnce = true;
                }

                // Only draw and move the alien if it is alive
                else if (tempAlienRow[j]->isAlive()) {
                    if (movingLeft){tempAlienRow.at(j)->moveLeft();}
                    else {tempAlienRow.at(j)->moveRight();}
                    if (tempAlienRow[j]->getY() >= 390) reachedBunker = true;
                }
                else if (tempAlienRow[j]->eraseOnce){
                    tempAlienRow[j]->eraseExplosion();
                    tempAlienRow[j]->eraseOnce = false;
                }
            }
        }
    }
    // move the aliens down if needed
    if (goDown) {
        for (int i = 0; i < 5; i++) {
            std::vector<Alien *> tempAlienRow = this->aliens.at(i);
            for (int j = 0; j < 11; j++) {
                // Only move down if alive
                if(tempAlienRow[j]->isAlive()) {
                    tempAlienRow.at(j)->moveDown();
                }
            }
        }
    }

    // Manage when the aliens shoot bullets
    if (alienFireTimer >= this->fireTickMax && Globals::getBullets().getNumAlienBullets() < 4) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 10);

        std::random_device rd2;
        std::mt19937 gen2(rd2());
        std::uniform_int_distribution<> dis2(0, 4);

        // Generate a random number in the range 1-11
        int randCol = dis(gen);
        int randRow;
        std::vector<Alien *> tempAlienRow;
        Alien *tempAlien;
        while (1) {
            randRow = dis2(gen2);
            tempAlienRow = this->aliens.at(randRow);
            tempAlien = tempAlienRow.at(randCol);
            if(tempAlien->isBottomAlien()) { break; }

        }
        if (tempAlien->isAlive() && tempAlien->isBottomAlien()) {   
            Globals::getBullets().newEnemyBullet(tempAlien);
            Globals::getBullets().incNumAlienBullets();
            alienFireTimer = 0;
            this->generateRandomFireDelay();
        }
    }

    goDown = false;
    secondsTimerD1 = secondsTimer;

    return false;
}

// Check for collisions between player bullet and aliens, killing player
// bullet and alien when they are overlapping.
void Aliens::checkCollisions() {
    // /exit loop if no flying player bullet 
    if (Globals::getBullets().getPlayerBullet() == NULL) return;
    
    // Loop through the vector of aliens
    for (int i = 0; i < 5; i++) {
        std::vector<Alien*> tempAlienRow = this->aliens.at(i);
        for (int j = 0; j < 11; j++) {
            // Only check collisions for alive aliens
            auto bullet = Globals::getBullets().getPlayerBullet();
            if (tempAlienRow[j]->isAlive() && tempAlienRow[j]->isOverlapping(bullet)) {
                numAliensAlive--;
                Globals::getBullets().kill(bullet);
                tempAlienRow[j]->explode();
                int counter = i;
                if(tempAlienRow[j]->isBottomAlien()) {
                    while(1) {
                        if(counter == 0) { break; }
                        std::vector<Alien*> aboveAlienRow = this->aliens.at(counter-1);
                        Alien* nextAlien = aboveAlienRow.at(j);
                        if(nextAlien->isAlive()) {
                            nextAlien->changeBottomStatus();
                            break;
                        }
                        counter--;
                    }
                }
                // Calculate score based on the type of alien that was killed
                if(i == 0) {  Globals::getScore().hitTopAlien(); }
                else if(i == 1 || i == 2) { Globals::getScore().hitMidAlien(); }
                else { Globals::getScore().hitBotAlien(); }
            }
        }
    }
}

// Generate a random number of ticks for firing the next alien bullet and
// store in fireTickMax
void Aliens::generateRandomFireDelay() {
    srand((unsigned) time(NULL));
    this->fireTickMax = rand() % 250;
}
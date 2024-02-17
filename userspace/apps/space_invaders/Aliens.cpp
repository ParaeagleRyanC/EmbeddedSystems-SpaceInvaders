#include "Aliens.h"
#include "Globals.h"

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
    alienFireTimer++;
    clockTimer++;
    secondsTimer = (clockTimer * 0.05);
    updateMovingDirection();
    
    if(secondsTimer > secondsTimerD1)
    {
        for(int i = 0; i < 5; i++)
        {
            std::vector<Alien *> tempAlienRow = this->aliens.at(i);
            for (int j = 0; j < 11; j++)
            {
                if(movingLeft){tempAlienRow.at(j)->moveLeft();}
                else{tempAlienRow.at(j)->moveRight();}
                if(goDown)
                {
                    tempAlienRow.at(j)->moveDown();
                }
            }
        }
    }

    if(alienFireTimer >= this->fireTickMax && Globals::getBullets().getNumAlienBullets() < 4)
    {
        srand((unsigned) time(NULL));
        int randCol = rand() % 11;
        int row = numAliensInCol[randCol] - 1;
        std::vector<Alien *> tempAlienRow = this->aliens.at(row);
        Alien *tempAlien = tempAlienRow.at(randCol);
        Globals::getBullets().newEnemyBullet(tempAlien);
        Globals::getBullets().incNumAlienBullets();
        alienFireTimer = 0;
        this->generateRandomFireDelay();
    }

    goDown = false;
    secondsTimerD1 = secondsTimer;


    return false;
}

// Check for collisions between player bullet and aliens, killing player
// bullet and alien when they are overlapping.
void Aliens::checkCollisions() {

}

// Generate a random number of ticks for firing the next alien bullet and
// store in fireTickMax
void Aliens::generateRandomFireDelay() {
    srand((unsigned) time(NULL));
    this->fireTickMax = rand() % 500;
}
#include "UFO.h"
#include "config.h"
#include "system.h"
#include "Globals.h"
#include "Bullet.h"

#include <time.h>
#include <stdio.h>

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
                if(!isAlive()) this->resurrect();
                // Reappear
                else this->draw();
                tickCnt = 0;
                state = MOVING;
                Globals::getPlaySound().loopOn();
                Globals::getPlaySound().setUfoPlaying();
                Globals::getPlaySound().playUfo();
            }
            break;

        // Moving State
        case MOVING:
            // Move over more
            if (tickCnt >= moveTickMax) {
                this->move(Globals::getSprites().getUFO(), UFO_MOVE_X_DISTANCE, 0);
                tickCnt = 0;
                return true;
            }
            // Reach the end of the screen
            else if (getX() + getWidth() > GRAPHICS_WIDTH) {
                this->move(Globals::getSprites().getUFO(), -getX(), 0);
                this->erase();
                state = HIDDEN;
                Globals::getPlaySound().loopOff();
                Globals::getPlaySound().setUfoPlaying();
            }
            // Get destroyed
            else if (flagKill) {
                this->move(Globals::getSprites().getAlienExplosion(), 0, 0);
                tickCnt = 0;
                flagKill = false;
                state = EXPLODING;
                Globals::getPlaySound().loopOff();
                Globals::getPlaySound().setUfoPlaying();
                Globals::getPlaySound().playUfoDie();
            }
            break;
        // Exploding State
        case EXPLODING:
            if (tickCnt >= (ALIENS_MOVE_DELAY_SECONDS / SYSTEM_FIT_PERIOD_SECONDS)) {
                this->eraseExplosion();
                this->kill();
                this->move(Globals::getSprites().getUFO(), -getX(), 0);
                this->erase();
                tickCnt = 0;
                state = HIDDEN;
            }
            break;
    }
    return false;
}

// Check for collisions between the player bullet and the UFO
void UFO::checkCollisions() {
    if (state == MOVING) {
        // exit loop if no flying player bullet 
        if (Globals::getBullets().getPlayerBullet() == NULL) return;

        auto bullet = Globals::getBullets().getPlayerBullet();
        if (this->isOverlapping(bullet)) {
            Globals::getBullets().kill(bullet);
            flagKill = true;
            Globals::getScore().hitSpaceship();
        }
    }
}
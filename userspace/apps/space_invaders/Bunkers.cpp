#include "Bunkers.h"
#include "Globals.h"
#include <iostream>

Bunkers::Bunkers() {
    for (int i = 0; i < BUNKER_COUNT; i++)
        bunkers.push_back(new Bunker(BUNKER_X_START + BUNKER_SPACING * i, BUNKER_Y_POS));
}


// Draw the bunker
void Bunkers::draw() {
    for (int i = 0; i < BUNKER_COUNT; i++)
        Globals::getGraphics().drawSprite(Globals::getSprites().getBunker(), 
            BUNKER_X_START + BUNKER_SPACING * i, BUNKER_Y_POS, BUNKER_SIZE, 
            Globals::getColorGreen(), Globals::getBackgroundColor());     
}

// Check collision between both player and enemy bullets and all bunkers.
void Bunkers::checkCollisions() {
    // loop through each bunker
    for (int i = 0; i < BUNKER_COUNT; i++) {

        // check if player bullet hits the bunker
        if (Globals::getBullets().getPlayerBullet() != nullptr)
            if (bunkers[i]->checkBulletCollision(Globals::getBullets().getPlayerBullet())) {
               // std::cout << "x: " << bunkers[i]->getX() << " y: " << bunkers[i]->getY() << std::endl;
                Globals::getBullets().kill(Globals::getBullets().getPlayerBullet());
            }
        
        // exit loop if no flying enemy bullet 
        // if (Globals::getBullets().getEnemyBullets().empty()) return;

        // // loop through each live bullet and check for collision with the bunker
        // for (auto blt = Globals::getBullets().getEnemyBullets().begin();
        //           blt != Globals::getBullets().getEnemyBullets().end();) {
        //     if (bunkers[i]->checkBulletCollision(*blt)) {
        //         Globals::getBullets().kill(*blt);
        //         blt--;
        //     }
        //     blt++;
        // }
    }
}
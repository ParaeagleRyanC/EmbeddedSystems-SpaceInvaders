#include "Bunkers.h"
#include "Globals.h"

Bunkers::Bunkers() {
    // do something with the bunkers vector
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
    // do something
}
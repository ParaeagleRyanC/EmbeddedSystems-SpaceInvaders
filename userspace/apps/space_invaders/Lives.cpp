#include "Lives.h"
#include "Globals.h"

Lives::Lives() {
    numLives = LIVES_AT_START;
    xDrawTankStart = 465;
}

// Draw the lives at the start of the game
void Lives::draw() { 
    Globals::getGraphics().drawStr("LIVES", 385, 8, 2, Globals::getColorWhite()); 
    for (int i = 0; i < numLives; i++)
        Globals::getGraphics().drawSprite(Globals::getSprites().getTank(SPRITE_TANK), 
        xDrawTankStart + TANK_LIVE_SPACING * i, 3, TANK_SIZE, Globals::getColorGreen(), Globals::getBackgroundColor());
}

// Whether the game is over due to running out of lives
bool Lives::isGameOver() { return numLives == 0; }

// Trigger losing a life and erasing a tank sprite
void Lives::loseALife() { 
    Globals::getGraphics().drawSprite(Globals::getSprites().getTank(SPRITE_TANK), 
                xDrawTankStart + TANK_LIVE_SPACING * (numLives - 1), 3, TANK_SIZE, Globals::getBackgroundColor(), Globals::getBackgroundColor());
    numLives--; 
}

// Trigger gaining a life (happens when all aliens are destroyed); however,
// make sure not to increase lives past the max number of lives.
void Lives::gainALife() { 
    if ((numLives + 1) > LIVES_MAX) return;
    numLives++; 
    drawNewLife();
}

// Draw a new life.  If tankLives is empty, you can use xDrawTankStart to
// determine where to draw it, otherwise, just get the last tank in the
// tankLives list and use that x-coordinate to calcuate where to draw the next
// one.
void Lives::drawNewLife() {
    if (numLives == 0) Globals::getGraphics().drawSprite(Globals::getSprites().getTank(SPRITE_TANK), xDrawTankStart, 3, TANK_SIZE, Globals::getColorGreen(), Globals::getBackgroundColor());
    else Globals::getGraphics().drawSprite(Globals::getSprites().getTank(SPRITE_TANK), 
                xDrawTankStart + TANK_LIVE_SPACING * (numLives - 1), 3, TANK_SIZE, Globals::getColorGreen(), Globals::getBackgroundColor());
}
#include "BunkerBlock.h"
#include "Globals.h"

// Construct a block at a given x,y.  
// Make sure this calls the parent constructor.
BunkerBlock::BunkerBlock(uint16_t x, uint16_t y) : GameObject(Globals::getSprites().getBunkerDmg(0), x, y, BUNKER_SIZE, Globals::getBackgroundColor()) {
    dmgLevel = 0;
}

// Increase the damage level, and draw the appropriate damage.
void BunkerBlock::inflictDamage() {
    dmgLevel++;
    Globals::getGraphics().drawSprite(Globals::getSprites().getBunkerDmg(dmgLevel), x, y, BUNKER_SIZE, Globals::getBackgroundColor());
    if (dmgLevel == BUNKER_BLOCK_MAX_DAMAGE) this->kill();
}
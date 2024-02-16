#include "Bullet.h"
#include "Globals.h"

 // Player bullet. Make sure this calls the parent constructor.
Bullet::Bullet(Tank *tank) : GameObject(Globals::getSprites().getBunker(), x, y, BUNKER_SIZE, Globals::getColorGreen()) {

}

// Enemy bullet. Make sure this calls the parent constructor.
Bullet::Bullet(Alien *alien, Sprite *sprite1, Sprite *sprite2): GameObject(Globals::getSprites().getBunker(), x, y, BUNKER_SIZE, Globals::getColorGreen()) {

}


 // Tick the bullet, and return whether it moved.
bool Bullet::tick() {
    return false;
}

// Kill this bullet (call the parent function kill(), but also notify Bullets
// that the bullet was destroyed)
void Bullet::kill() {

}
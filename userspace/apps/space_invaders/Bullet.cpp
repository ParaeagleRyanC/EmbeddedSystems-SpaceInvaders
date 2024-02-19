#include "Globals.h"
#include "Bullet.h"
#include "Tank.h"
#include "Alien.h"
#include <stdint.h>

#define OFFSET_BULLET_Y 10

Bullet::Bullet(Tank *tank) : 
    GameObject(Globals::getSprites().getBullet(SPRITE_BULLET_TANK), tank->getMidX(), tank->getY() - OFFSET_BULLET_Y,
                BULLET_SIZE,{255, 255, 255}),
    usingSprite1(true),
    playerBullet(true),
    sprite1(Globals::getSprites().getBullet(SPRITE_BULLET_TANK)),
    sprite2(Globals::getSprites().getBullet(SPRITE_BULLET_TANK)) {}

Bullet::Bullet(Alien *alien, Sprite *sprite1, Sprite *sprite2) : 
    GameObject(sprite1,alien->getMidX(), alien->getY()+15,
                BULLET_SIZE,{255, 255, 255}),
    usingSprite1(true),
    playerBullet(false),
    sprite1(sprite1),
    sprite2(sprite2) {}

bool Bullet::tick() {    
    //do these operations for the player bullet
    if (playerBullet){
        if (this->isAlive() && this->getY() <= 20) { this->kill(); }
        if (usingSprite1 && this->isAlive()) {  this->move(sprite1,0,-2); }
    }
    //do these operations for the alien bullets
    else {
        if (this->isAlive() && this->getY() >= GRAPHICS_HEIGHT-20) {
            this->kill();
            Globals::getBullets().decNumAlienBullets();
        }
        if (usingSprite1 && this->isAlive()) {
            this->move(sprite1,0,2);
            usingSprite1 = !usingSprite1;
        }
        else if (!usingSprite1 && this->isAlive()) {
            this->move(sprite2,0,2);
            usingSprite1 = !usingSprite1;
        }
        
    }
    return true;
}

void Bullet::killBullet() { this->kill(); }
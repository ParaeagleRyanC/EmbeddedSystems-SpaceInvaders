#include "Globals.h"
#include "Bullet.h"
#include "Tank.h"
#include "Alien.h"
#include <stdint.h>

#define OFFSET_BULLET_Y 10

Bullet::Bullet(Tank *tank) : 
    GameObject(Globals::getSprites().getBullet(SPRITE_BULLET_TANK),tank->getMidX(), tank->getY() - OFFSET_BULLET_Y,
                BULLET_SIZE,{255, 255, 255}),
    usingSprite1(true),
    playerBullet(true),
    sprite1(Globals::getSprites().getBullet(SPRITE_BULLET_TANK))
{}

Bullet::Bullet(Alien *alien, Sprite *sprite1, Sprite *sprite2) : 
    GameObject(sprite1,alien->getMidX(), alien->getY(),
                1,{255, 255, 255}),
    usingSprite1(true),
    playerBullet(false),
    sprite1(sprite1),
    sprite2(sprite2)
{}

bool Bullet::tick()
{
    // bool flag = this->isAlive();
    // // std::cout << flag << std::endl;
    std::cout << "boo" << std::endl;
    if(this->isAlive())
    {
        std::cout << "we on the inside" << std::endl;
        this->kill();
    }
    std::cout << "Getting there" << std::endl;
    if(usingSprite1 && this->isAlive())
    {
        this->move(sprite1,-1,-1);
    }
    else if (!usingSprite1 && this->isAlive())
    {
        this->move(sprite2,-1,-1);
    }
    std::cout << "Getting everywhere" << std::endl;
    return true;
}

void Bullet::killBullet()
{
    this->kill();
}
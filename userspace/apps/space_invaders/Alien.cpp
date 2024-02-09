#include "Alien.h"
#include <stdint.h>
#include <GameObject.h>
#include <iostream>

Alien::Alien(Sprite *spriteIn, Sprite *spriteOut, uint16_t x, uint16_t y) :
    GameObject(spriteIn, x, y, 2, {255, 255, 255}), // Call base class constructor in the initializer list
    spriteIn(spriteIn), // Initialize member variables
    spriteOut(spriteOut),
    in(true),
    exploding(false)
{
}

void Alien::moveLeft()
{
    if(in){move(spriteIn,-1,0);}
    else{move(spriteOut,-1,0);}
}

void Alien::moveRight()
{
    if(in){move(spriteIn,+1,0);}
    else{move(spriteOut,+1,0);}
}

void Alien::moveDown()
{
    if(in){move(spriteIn,0,-1);}
    else{move(spriteOut,0,-1);}
}

void Alien::explode()
{
    return;
}
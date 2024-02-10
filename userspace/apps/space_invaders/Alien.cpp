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
    this->in = !in;
    if(in){move(spriteIn,-3,0);}
    else{move(spriteOut,-3,0);}
}

void Alien::moveRight()
{
    this->in = !in;
    if(in){move(spriteIn,3,0);}
    else{move(spriteOut,3,0);}
}

void Alien::moveDown()
{
    this->in = !in;
    if(in){move(spriteIn,0,20);}
    else{move(spriteOut,0,20);}
}

void Alien::explode()
{
    return;
}
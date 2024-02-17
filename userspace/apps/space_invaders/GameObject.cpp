#include "GameObject.h"
#include "Sprite.h"
#include <stdint.h>
#include <iostream>
#include "Globals.h"


GameObject::GameObject(Sprite *sprite, uint16_t x, uint16_t y, uint8_t size, rgb_t color)
{
    this->size = size;
    this->color = color;
    this->x = x;
    this->y = y;

    this->sprite = sprite;
    alive = true;
}

void GameObject::draw()
{
    Globals::getGraphics().drawSprite(sprite, x, y, size, color, Globals::getBackgroundColor());
}

void GameObject::erase()
{
    Globals::getGraphics().drawSprite(sprite, x, y, size, Globals::getBackgroundColor(), Globals::getBackgroundColor());
}

void GameObject::move(Sprite *newSprite, int16_t delta_x, int16_t delta_y)
{
    this->erase();
    x += delta_x;
    y += delta_y;
    Globals::getGraphics().drawSprite(newSprite, x, y, this->size, this->color, Globals::getBackgroundColor());
}

void GameObject::kill()
{
    alive = false;
    this->erase();
}

void GameObject::resurrect()
{
    alive = true;
    this->draw();
}

// Draw the object, but don't draw the background pixels 
// (this is slower, but is needed for the bunker damage)
void GameObject::drawNoBackground() {
    Globals::getGraphics().drawSprite(sprite, x, y, this->size, Globals::getColorGreen());
}

bool isOverlapped(uint16_t pos, uint16_t boundary1, uint16_t boundary2) {
    return pos >= boundary1 && pos <= boundary2;
}

// Check if this object is overlapping a given object.  This can be used to
// handle all collisions in the game.
bool GameObject::isOverlapping(GameObject *object) {
    if (object == nullptr || !this->isAlive() || !object->isAlive()) return false;

    uint16_t object1Left = this->getX();
    uint16_t object1Right = this->getX() + this->getWidth();
    uint16_t object1Top = this->getY();
    uint16_t object1Bottom = this->getY() + this->getHeight();

    uint16_t object2Left = object->getX();
    uint16_t object2Right = object->getX() + object->getWidth();
    uint16_t object2Top = object->getY();
    uint16_t object2Bottom = object->getY() + object->getHeight();

    // std::cout << "Object1Left: " << object1Left << " Object1Right: " << object1Right << 
    //             " Object1Top: " << object1Top << " Object1Bottom: " << object1Bottom << std::endl << std::endl;

    // std::cout << "Object2Left: " << object2Left << " Object2Right: " << object2Right << 
    //             " Object2Top: " << object2Top << " Object2Bottom: " << object2Bottom << std::endl << std::endl;

    if (isOverlapped(object1Left, object2Left, object2Right) || 
        isOverlapped(object1Right, object2Left, object2Right))
        if (isOverlapped(object1Top, object2Top, object2Bottom) || 
            isOverlapped(object1Bottom, object2Top, object2Bottom))
                return true; 

    if (isOverlapped(object2Left, object1Left, object1Right) || 
        isOverlapped(object2Right, object1Left, object1Right))
        if (isOverlapped(object2Top, object1Top, object1Bottom) || 
            isOverlapped(object2Bottom, object1Top, object1Bottom))
                return true; 

    return false;
}


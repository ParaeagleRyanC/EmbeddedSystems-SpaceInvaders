#include "GameObject.h"
#include "Sprite.h"
#include <stdint.h>
#include "Globals.h"


GameObject::GameObject(Sprite *sprite, uint16_t x, uint16_t y, uint8_t size, rgb_t color)
{
    this->size = size;
    this->color = color;
    this->x = x;
    this->y = y;

    this->sprite = sprite;
}

void GameObject::draw()
{
    Globals::getGraphics().drawSprite(sprite, x, y, size, color, Globals::getBackgroundColor());
}

void GameObject::erase()
{
    Globals::getGraphics().drawSprite(sprite, x, y, size, Globals::getBackgroundColor());
}

void GameObject::move(Sprite *newSprite, int16_t delta_x, int16_t delta_y)
{
    this->erase();
    Globals::getGraphics().drawSprite(newSprite, delta_x, delta_y, this->size, this->color);
}

void GameObject::resurrect()
{
    alive = true;
    this->draw();
}


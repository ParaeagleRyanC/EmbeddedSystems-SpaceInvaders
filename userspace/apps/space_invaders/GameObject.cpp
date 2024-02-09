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


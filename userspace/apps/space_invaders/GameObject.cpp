#include "GameObject.h"
#include "Sprite.h"
#include <stdint.h>
#include "Graphics.h"
#include "Colors.h"


static Graphics graphics;
static rgb_t green = Colors::GREEN;
static rgb_t white = Colors::WHITE;
static rgb_t black = Colors::BLACK;

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
    graphics.drawSprite(sprite, x, y, size, color, black);
}

void GameObject::erase()
{
    graphics.drawSprite(sprite, x, y, size, black);
}

void GameObject::move(Sprite *newSprite, int16_t delta_x, int16_t delta_y)
{
    this->erase();
    graphics.drawSprite(newSprite, delta_x, delta_y, this->size, this->color);
}

void GameObject::resurrect()
{
    alive = true;
    this->draw();
}


#include "Sprite.h"

// Construct a new sprite from bitmap data
Sprite::Sprite(const uint32_t data[], uint8_t height, uint8_t width) {
    this->data = data;
    this->height = height;
    this->width = width;
}

// Return whether the given row and col idx has a pixel in the sprite.
bool Sprite::isFgPixel(uint8_t row, uint8_t col) {
    uint32_t newPixel = (row * width) + col;
    return data[newPixel];
}
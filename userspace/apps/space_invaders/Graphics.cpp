
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <iostream>
#include <cassert>

#include "Globals.h"
#include "Graphics.h"
#include "Sprites.h"
#include "Sprite.h"
#include "Colors.h"
#include "system.h"
#include "resources/sprites.h"

// initialize hdmi and draw a sprite

#define OPEN_ERROR -1
#define SPACING 10
#define BYTES_PER_PIXEL 3

// initialize hdmi
Graphics::Graphics() {
    // open the device
    fd = open(SYSTEM_HDMI_FILE, O_RDWR);
    if (fd == OPEN_ERROR) {
        printf("HDMI Init error -- did you forget to sudo?\n");
    }
}

// Fill the screen.  This is fastest if you write line by line.
void Graphics::fillScreen(rgb_t color) {
    uint32_t totalNumPixels = GRAPHICS_WIDTH * GRAPHICS_HEIGHT;
    rgb_t* screenBuffer = new rgb_t[totalNumPixels];

    for (uint32_t i = 0; i < totalNumPixels; i++)
        screenBuffer[i] = color;

    write(fd, screenBuffer, totalNumPixels * sizeof(rgb_t));
    delete[] screenBuffer;
}

// Fill the screen.  This is fastest if you write line by line.
void Graphics::drawLine(rgb_t color) {
    rgb_t* screenBuffer = new rgb_t[GRAPHICS_WIDTH];

    for (uint32_t i = 0; i < GRAPHICS_WIDTH; i++)
        screenBuffer[i] = color;

    write(fd, screenBuffer, GRAPHICS_WIDTH * sizeof(rgb_t));
    delete[] screenBuffer;
}

// This draws a sprite of given size and color at an x,y location.  This
// version of the function is given a background color (bgColor), such that
// every pixel of sprite region is written (with either color or bgColor).
// This is faster because it allows you to write line by line and minimize
// system calls.
void Graphics::drawSprite(Sprite *sprite, uint16_t x, uint16_t y, uint8_t size,
                rgb_t color, rgb_t bgColor) {

    lseek(fd, GRAPHICS_WIDTH * y * 3 + x * 3, SEEK_SET);
    uint8_t spriteWidth = sprite->getWidth();
    uint8_t spriteHeight = sprite->getHeight();

    if(size == 1)
    {
        for (int i = 0; i < spriteHeight; i++) {
            uint32_t sprite_col = 0;
            char line[spriteWidth * 3 * size];
            for (int j = 0; j < spriteWidth * 3; j += 3) {
            if (sprite->isFgPixel(i, sprite_col)) {
                line[j] = color.r;
                line[j+1] = color.g;
                line[j+2] = color.b;
                //   oss << "1 ";
            } else {
                line[j] = bgColor.r;
                line[j+1] = bgColor.g;
                line[j+2] = bgColor.b;
            }
            sprite_col++;
            }
            write(fd, line, sizeof(line));
            lseek(fd, 3 * (GRAPHICS_WIDTH - spriteWidth), SEEK_CUR);
        }
    }
    else {
        for (int i = 0; i < spriteHeight; i++) {
            uint32_t sprite_col = 0;
            char line[spriteWidth * 3 * size];
            std::vector<char> tempLine;
            for (int j = 0; j < spriteWidth * 3; j += 3) {
                if (sprite->isFgPixel(i, sprite_col)) {
                    for(int l = 0; l < size; l++)
                    {
                        tempLine.push_back(color.r);
                        tempLine.push_back(color.g);
                        tempLine.push_back(color.b);
                    }
                    //   oss << "1 ";
                } else {
                    for(int l = 0; l < size; l++)
                    {
                        tempLine.push_back(bgColor.r);
                        tempLine.push_back(bgColor.g);
                        tempLine.push_back(bgColor.b);
                    }
                }
                sprite_col++;
            }
            for (int j = 0; j < spriteWidth * 3 * size;j++)
            {
                line[j] = tempLine.at(j);
            }
            for (int j = 0; j < size; j++){
                write(fd, line, sizeof(line));
                lseek(fd, 3 * (GRAPHICS_WIDTH - spriteWidth * size), SEEK_CUR);
            }
        }
    }
}

// Same as previous function, but does not write over the background pixels.
// Although this writes fewer pixels, it often requires more system calls and
// so will be slower.  This is needed to draw the bunker damage.
void Graphics::drawSprite(Sprite *sprite, uint16_t x, uint16_t y, uint8_t size,
                rgb_t color) {
    uint8_t width = sprite->getWidth();
    uint8_t height = sprite->getHeight();

    for(uint8_t row = 0; row < height; row++)
    {
        for(uint8_t col = 0; col < width;col++)
        {
            if(!sprite->isFgPixel(row, width-col-1)){continue;}
            uint32_t pixel = color.r | (color.g << 8) | (color.b << 8);

            for(uint8_t x_size = 0; x_size < size; x_size++)
            {
                for (uint8_t y_size = 0; y_size < size;y_size++)
                {
                    lseek(fd,
                            (y + row * size + y_size) * GRAPHICS_WIDTH * BYTES_PER_PIXEL + (x + col * size + x_size) * BYTES_PER_PIXEL, SEEK_SET);
                    assert(write(fd,&pixel,BYTES_PER_PIXEL) == BYTES_PER_PIXEL);        
                }
            }
        }
    }
}

// Draws a string on the screen, and returns the width
uint16_t Graphics::drawStr(std::string str, uint16_t x, uint16_t y, uint8_t size,
                rgb_t color) {

    // iterate through each char in the str
    for (uint16_t i = 0; i < str.length(); i++) {
        char c = str[i];
        
        // default invalid characters to a space
        if (!std::isalpha(c) && !std::isdigit(c) && !std::isspace(c)) 
            c = ' ';

        // if char is a letter, make uppercase
        if (std::isalpha(c)) 
            c = toupper(c);

        // get the sprite with a given char
        Sprite* sprite = Globals::getSprites().getChar(c);

        // calculate the new x position
        x = x + (SPRITES_5X5_ROWS * size + 2);

        rgb_t black = Colors::BLACK;
        
        // call draw sprite to draw the sprite with default background as RED
        drawSprite(sprite, x, y, size, color, black);
    }

    return getStrWidth(str.length(), size);
}

// Draws a string on the screen that is centered horizontally
void Graphics::drawStrCentered(std::string str, uint16_t y, uint8_t size, rgb_t color) {

    // get the width of the str
    uint16_t strWidth = getStrWidth(str.length(), size);

    // find the x position to start
    uint16_t xStart = (GRAPHICS_WIDTH - (strWidth / 2)) / 2;

    // call drawStr function to actually draw the str
    drawStr(str, xStart, y, size, color);
}

// Returns the width of a str of given length and size.  This should take into
// account the character sizes, and spacing between characters.  For a given
// string and size, this should return the same value as drawStr.
uint16_t Graphics::getStrWidth(uint8_t strLen, uint8_t size) {
    return strLen * size * SPRITES_5X5_ROWS + (strLen - 1) * size * SPACING;
}
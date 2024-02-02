
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <iostream>

#include "Graphics.h"
#include "Sprites.h"
#include "Sprite.h"
#include "Colors.h"
#include "system.h"
#include "resources/sprites.h"

// initialize hdmi and draw a sprite

#define OPEN_ERROR -1
#define SPACING 10

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

// This draws a sprite of given size and color at an x,y location.  This
// version of the function is given a background color (bgColor), such that
// every pixel of sprite region is written (with either color or bgColor).
// This is faster because it allows you to write line by line and minimize
// system calls.
void Graphics::drawSprite(Sprite *sprite, uint16_t x, uint16_t y, uint8_t size,
                rgb_t color, rgb_t bgColor) {

    lseek(fd,GRAPHICS_WIDTH*y*3 + x*3,SEEK_SET);
    uint8_t spriteWidth = sprite->getWidth();
    uint8_t spriteHeight = sprite->getHeight();

    for (int i = 0; i < spriteHeight; i++)
    {
        uint32_t sprite_col = 0;
        char line[spriteWidth*3];
        for (int j = 0; j < spriteWidth * 3;j+=3)
        {
            if(sprite->isFgPixel(i,sprite_col))
            {
                line[j] = color.r;
                line[j + 1] = color.g;
                line[j + 2] = color.b;
                //std::cout << "1 ";
            }
            else
            {
                line[j] = bgColor.r;
                line[j + 1] = bgColor.g;
                line[j + 2] = bgColor.b;
                //std::cout << "0 ";
            }
            sprite_col++;
        }
        //std::cout << std::endl;
        write(fd,line,sizeof(line));
        lseek(fd,3*(GRAPHICS_WIDTH - spriteWidth),SEEK_CUR);
    }
}

// Same as previous function, but does not write over the background pixels.
// Although this writes fewer pixels, it often requires more system calls and
// so will be slower.  This is needed to draw the bunker damage.
void Graphics::drawSprite(Sprite *sprite, uint16_t x, uint16_t y, uint8_t size,
                rgb_t color) {
    lseek(fd,GRAPHICS_WIDTH*y*3 + x*3,SEEK_SET);
    uint8_t spriteWidth = sprite->getWidth();
    uint8_t spriteHeight = sprite->getHeight();

    for (int i = 0; i < spriteHeight; i++)
    {
        uint32_t sprite_col = 0;
        //char line[spriteWidth*3];
        for (int j = 0; j < spriteWidth * 3;j+=3)
        {
            char pixel[3];
            if(sprite->isFgPixel(i,sprite_col))
            {
                pixel[0] = color.r;
                pixel[1] = color.g;
                pixel[2] = color.b;
                write(fd, pixel, 3);
            }
            else
            {
                lseek(fd,3,SEEK_CUR);
            }
            sprite_col++;
        }
        lseek(fd,3*(GRAPHICS_WIDTH - spriteWidth),SEEK_CUR);
    }
}

// Draws a string on the screen, and returns the width
uint16_t Graphics::drawStr(std::string str, uint16_t x, uint16_t y, uint8_t size,
                rgb_t color) {
    
    Sprites sprites;

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
        Sprite* sprite = sprites.getChar(c);

        // calculate the new x position
        x = x + (SPRITES_5X5_ROWS * size + 2);
        //std::cout << x << std::endl;
        
        // call draw sprite to draw the sprite with default background as RED
        drawSprite(sprite, x, y, size, color, Colors::BLACK);
    }

    return getStrWidth(str.length(), size);
}

// Draws a string on the screen that is centered horizontally
void Graphics::drawStrCentered(std::string str, uint16_t y, uint8_t size, rgb_t color) {

    // get the width of the str
    uint16_t strWidth = getStrWidth(str.length(), size);

    // find the x position to start
    uint16_t xStart = (GRAPHICS_WIDTH - strWidth) / 2;

    // call drawStr function to actually draw the str
    drawStr(str, xStart, y, size, color);
}

// Returns the width of a str of given length and size.  This should take into
// account the character sizes, and spacing between characters.  For a given
// string and size, this should return the same value as drawStr.
uint16_t Graphics::getStrWidth(uint8_t strLen, uint8_t size) {
    return strLen * size * SPRITES_5X5_ROWS + (strLen - 1) * SPACING;
}
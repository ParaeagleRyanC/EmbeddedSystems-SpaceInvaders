
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <iostream>

#include "Graphics.h"
#include "system.h"

// initialize hdmi and draw a sprite

#define OPEN_ERROR -1

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

}

// Same as previous function, but does not write over the background pixels.
// Although this writes fewer pixels, it often requires more system calls and
// so will be slower.  This is needed to draw the bunker damage.
void Graphics::drawSprite(Sprite *sprite, uint16_t x, uint16_t y, uint8_t size,
                rgb_t color) {

}

// Draws a string on the screen, and returns the width
uint16_t Graphics::drawStr(std::string str, uint16_t x, uint16_t y, uint8_t size,
                rgb_t color) {
    return 0;
}

// Draws a string on the screen that is centered horizontally
void Graphics::drawStrCentered(std::string str, uint16_t y, uint8_t size, rgb_t color) {

}

// Returns the width of a str of given length and size.  This should take into
// account the character sizes, and spacing between characters.  For a given
// string and size, this should return the same value as drawStr.
uint16_t Graphics::getStrWidth(uint8_t strLen, uint8_t size) {
	uint16_t strWidth = 0;
	
	for (uint8_t i = 0; i < strLen; i++) {
		char c = str[i];
		uint16_t charWidth = size * 8;
		strWidth += charWidth;
	}
    return strWidth;
}
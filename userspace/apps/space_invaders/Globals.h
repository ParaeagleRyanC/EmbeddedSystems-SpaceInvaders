#ifndef GLOBALS_H
#define GLOBALS_H

#include "Bullets.h"
#include "Graphics.h"
#include "Lives.h"
#include "Score.h"
#include "Sprites.h"
#include "system.h"
#include "Bunkers.h"
#include "UFO.h"

// Track all global variables as static members of this class
class Globals {
public:
  Globals();

private:
  static Graphics graphics;
  static Sprites sprites;
  static rgb_t backgroundColor;
  static rgb_t colorWhite;
  static rgb_t colorGreen;
  static rgb_t colorRed;
  static Bullets bullets;
  static Bunkers bunkers;
  static Score score;
  static Lives lives;
  static UFO ufo;

public:
  Globals(const Globals &) = delete; // prevent Globals from become instantiated
  static Graphics &getGraphics() { return graphics; }
  static Sprites &getSprites() { return sprites; }
  static Bunkers &getBunkers() { return bunkers; }
  static rgb_t getBackgroundColor() { return backgroundColor; }
  static rgb_t getColorWhite() { return colorWhite; }
  static rgb_t getColorGreen() { return colorGreen; }
  static rgb_t getColorRed() { return colorRed; }
  static void setBackgroundColor(rgb_t color) { backgroundColor = color; }
  static Bullets &getBullets() { return bullets; }
  static double getTickPeriod() { return SYSTEM_FIT_PERIOD_SECONDS; }
  static Score &getScore() { return score; }
  static Lives &getLives() { return lives; }
  static UFO &getUFO() {return ufo; }
};

#endif /* GLOBALS_H */

#ifndef ALIEN_H
#define ALIEN_H

#include <stdint.h>

#include "GameObject.h"

class Sprite;

// Class that represents a single alien.
class Alien : public GameObject {
public:
  // Create a new Alien, with given "in" and "out" sprites that are displayed
  // when the alien moves. Make sure this calls the parent constructor.
  Alien(Sprite *spriteIn, Sprite *spriteOut, uint16_t x, uint16_t y, bool bottomStatus);

private:
  Sprite *spriteIn;
  Sprite *spriteOut;
  uint8_t size;

  // Whether the 'in' sprite is being displayed
  bool in;

  // Whether the alien is exploding.  When exploding it's not yet dead, but the
  // explosion sprite is displayed.
  bool exploding;
  bool bottom;

public:
  bool eraseOnce;
  
  // Move the alien left, right or down
  void moveLeft();
  void moveRight();
  void moveDown();

  // Trigger the alien to explode
  void explode();
  bool isExploding() { return exploding; }
  void resetExploding();
  bool isBottomAlien();
  void changeBottomStatus();
};

#endif /* ALIEN_H */

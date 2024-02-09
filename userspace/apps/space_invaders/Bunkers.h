#ifndef BUNKERS_H
#define BUNKERS_H

#include <vector>

#include "Bunker.h"
#include "config.h"

#define BUNKER_Y_POS 400
#define BUNKER_X_START 90
#define BUNKER_SPACING 135

// Tracks all Bunker objects in the game.
class Bunkers {
public:
  Bunkers();

private:
  std::vector<Bunker *> bunkers;

public:
  // Draw the bunker
  void draw();

  // Check collision between both player and enemy bullets and all bunkers.
  void checkCollisions();
};
#endif /* BUNKERS_H */

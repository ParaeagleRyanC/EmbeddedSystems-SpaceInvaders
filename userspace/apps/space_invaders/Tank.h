#ifndef TANK_H
#define TANK_H

#include <stdint.h>

#include "GameObject.h"

#define FIRE 0
#define MOVE_RIGHT 1
#define MOVE_LEFT 2

// The player tank (can also be used to draw the tanks for the lives)
class Tank : public GameObject {
public:
  // Player tank (initialized to starting location)
  Tank();

  // Tank for player lives, drawn at given x,y coordinate
  // Tank(uint16_t x, uint16_t y);

private:
  // Tick counter and max tick for moving the tank
  uint32_t tickCnt;
  const uint32_t tickMax;
  const uint32_t deathTickMax;

  // Tank state, which is either alive or switching between the two explosion
  // sprites for a given time.
  typedef enum {
    TANK_STATE_ALIVE,
    TANK_STATE_DYING1,
    TANK_STATE_DYING2
  } tank_state_t;
  tank_state_t state;

  // Used to flag the state machine that the tank should be destroyed.
  bool flagExplosion;

public:
  bool isTankHit() { return this-> flagExplosion; }

  // Tick the tank
  bool tick(uint8_t btn);

  // Check for collisions between alien bullets and the tank
  void checkCollisions();

  // Kill the tank.  This should flag the explosion (but don't call
  // GameObject.kill(), otherwise the tank won't be drawn)
  void kill();
};

#endif /* TANK_H */

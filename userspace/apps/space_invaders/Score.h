#ifndef SCORE_H
#define SCORE_H

#include <cstdlib>
#include <stdint.h>
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>

#include "config.h"

// Kepp track of and draw the game score
class Score {
public:
  Score() : score(0) {}

private:
  // The game score
  uint32_t score;

public:
  // Pad a score integer with 0s
  static std::string padScore(uint32_t score) {
    // add 0 paddings to integer
    std::stringstream ss;
    ss << std::setw(5) << std::setfill('0') << score;
    return ss.str();
  }

  uint32_t getScore() { return score; }

  // Draw the score at game start
  void draw(); 

  // Call the functions when aliens are hit.
  void hitTopAlien() {
    score += SCORE_TOP_ALIEN;
    draw();
  }
  void hitMidAlien() {
    score += SCORE_MID_ALIEN;
    draw();
  }
  void hitBotAlien() {
    score += SCORE_BOT_ALIEN;
    draw();
  }
  void hitSpaceship() {
    uint8_t randomNumber = (std::rand() % (SCORE_SPACESHIP_MAX / SCORE_SPACESHIP_MIN)) + 1;
    score += (randomNumber * SCORE_SPACESHIP_MIN);
    draw();
  }
};

#endif /* SCORE_H */

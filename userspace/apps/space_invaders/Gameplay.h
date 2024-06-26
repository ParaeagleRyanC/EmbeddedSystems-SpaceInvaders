#ifndef GAMEPLAY_H
#define GAMEPLAY_H

class Bunkers;
class Tank;
class Aliens;
class UFO;
class Bullets;
class HighScores;

// This class manages the normal gameplay of the game (but not the
// gameover/highscores screen)
class Gameplay {
public:
  Gameplay();

private:
  Bunkers *bunkers;
  Tank *tank;
  Aliens *aliens;
  UFO *ufo;
  Bullets *bullets;
  HighScores *highScores;
  bool saveHighscores;
  int gameVolume;

  // Check all collisions by calling all appropriate collision functions.  This
  // only needs to be done if some game object moved during the last tick.
  void checkCollisions();

public:
  bool isDoneSavingScores() { return saveHighscores; }

  // Draw everything at the beginning of the game
  void drawInit();

  // Tick the game
  void tick();

  // Tick the high scores screen
  void tickHighScores();

  void buttons_isr();

  void end_game();
  
  // Return whether the game is over (all lives lost, or aliens reach the
  // bottom)
  bool isGameOver();
};

#endif /* GAMEPLAY_H */

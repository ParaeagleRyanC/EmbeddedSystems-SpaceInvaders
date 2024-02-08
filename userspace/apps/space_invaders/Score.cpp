#include "Score.h"
#include "Globals.h"

// Draw the score at game start
void Score::draw() {
    std::string scoreString = "SCORE"; 
    Globals::getGraphics().drawStr(scoreString, 8, 5, 2, Globals::getColorWhite());
    Globals::getGraphics().drawStr(padScore(getScore()), 80, 5, 2, Globals::getColorGreen());
}
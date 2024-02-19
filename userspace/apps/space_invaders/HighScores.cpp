#include "HighScores.h"
#include "Graphics.h"
#include "Colors.h"
#include "Globals.h"

#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <string>
#include <cstring>
#include <algorithm> 
#include <iomanip>

#define USER_ENTRY_X 100
#define USER_ENTRY_Y 100
#define USER_ENTRY_SIZE 1

// helper function to get the filepath to highscore.txt
std::string getPathToHighScoreTxt() {
    // create a buffer to store the path, up to 1024 characters
    char path[1024]; 

    // Path to your symbolic link
    const char* symlinkPath = "/proc/self/exe";
    
    // Read the contents of the symbolic link into the path buffer
    ssize_t pathLength = readlink(symlinkPath, path, sizeof(path) - 1);

    // convert path buffer to string
    std::string fpath(path, pathLength);

    // find the index of the last slash
    size_t index = fpath.find("userspace");

    // replace the space_invaders exe with highscores.txt
    fpath.replace(index, pathLength - index, "userspace/apps/space_invaders/highscores.txt");

    return fpath;
}

// helper function to get the data in highscore.txt
std::vector<std::pair<std::string, uint32_t>> readHighScores(std::string filePath) {
    std::vector<std::pair<std::string, uint32_t>> data;

    // Open the text file
    std::ifstream inputFile(filePath);

    // Read lines from the file
    std::string line;
    while (std::getline(inputFile, line)) {
        // Assuming each line contains a string and a uint32_t separated by space
        std::istringstream iss(line);
        std::string str;
        uint32_t num;

        // Extract string and uint32_t from the line
        if (iss >> str >> num) {
            // Add the pair to the vector
            data.emplace_back(str, num);
        }
    }

    // Close the file
    inputFile.close();

    return data;
}

HighScores::HighScores(uint32_t score) {

    Globals::getGraphics().fillScreen(Globals::getBackgroundColor());
    this->score = score;
    userEntry = "AAA";
    entryIdx = 0;
    tickCnt = 0;
    tickCntMax = 30;
    showChar = true;
    nameEntryY = 0;
    filePath = getPathToHighScoreTxt();
    highScores = readHighScores(filePath);

}

// Used to draw the name entry screen.
void HighScores::drawUserEntry(std::string entry) {
    Globals::getGraphics().drawStr(entry, 296, USER_ENTRY_Y, USER_ENTRY_SIZE, Globals::getColorWhite());
}

// Tick the name entry screen
void HighScores::tickUserEntry(uint8_t btn) {
    // move cursor to the next letter
    if (btn == NEXT_BTN) entryIdx++;

    // increment letter
    if (btn == INC_LETTER) {
        if (userEntry[entryIdx] == 'Z') userEntry[entryIdx] = 'A';
        else userEntry[entryIdx] = userEntry[entryIdx] + 1;
    }

    // decrement letter
    if (btn == DEC_LETTER) {
        if (userEntry[entryIdx] == 'A') userEntry[entryIdx] = 'Z';
        else userEntry[entryIdx] = userEntry[entryIdx] - 1;
    }

    // flash the character
    tickCnt++;
    if (tickCnt == tickCntMax) {
        tickCnt = 0;
        if (showChar) {
            showChar = false;
            drawUserEntry(userEntry);
        }
        else {
            showChar = true;
            std::string temp = userEntry;
            temp[entryIdx] = ' ';
            drawUserEntry(temp);
        }
    }
}

// Triggers the highScores to be written out to the file.
void HighScores::save() {
    // put name and score into vector
    highScores.emplace_back(userEntry, score);

    // Sort the vector based on the integer in descending order
    std::sort(highScores.begin(), highScores.end(), [](const auto& a, const auto& b) {
        return a.second > b.second;
    });

    // draw highscores 
    drawHighScores();

    // Keep only the top 10 entries
    if (highScores.size() > 10) {
        highScores.erase(highScores.begin() + 10, highScores.end());
    }

    // Open the file in truncation mode to clear its contents
    std::ofstream outputFile(filePath, std::ofstream::trunc);

    // Write vector data to the file
    for (const auto& pair : highScores) {
        outputFile << pair.first << " " << pair.second << "\n";
    }
}

// Draw the high scores screen (shown after the user enters their initials, or
// immediately after game over if a new high score is not achieved)
void HighScores::drawHighScores() {

    rgb_t textColor = Globals::getColorWhite();
    bool highLightOnce = true;

    // erase "ENTER YOUR NAME"
    Globals::getGraphics().drawStrCentered("               ", 55, 3, textColor); // same size as ENTER YOUR NAME
    // write "HIGH SCORES"
    Globals::getGraphics().drawStrCentered("HIGH SCORES", 55, 3, textColor); // same size as ENTER YOUR NAME
    // erase name
    Globals::getGraphics().drawStr("   ", 296, USER_ENTRY_Y, 1, textColor); // same size as name

    uint16_t yPos = USER_ENTRY_Y;
    uint16_t yInc = 20;

    // loop through the vector
    for (const auto& pair : highScores) {
        // add 0 paddings to integer
        std::stringstream ss;
        ss << std::setw(5) << std::setfill('0') << pair.second;

        if (pair.first == userEntry && pair.second == score && highLightOnce) {
            textColor = Globals::getColorGreen();
            highLightOnce = false;
        }
        else textColor = Globals::getColorWhite();

        // combine name and score into one string
        std::string entry = pair.first + "   " + ss.str();
        
        // print to screen
        Globals::getGraphics().drawStr(entry, 275, yPos, USER_ENTRY_SIZE, textColor);

        // increment y position
        yPos += yInc;
    }
}

#include "HighScores.h"
#include "Graphics.h"
#include "Colors.h"

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


Graphics graphics = Graphics();
rgb_t textColor = Colors::WHITE;

// helper function to get the filepath to highscore.txt
std::string getPathToHighScoreTxt() {
    // create a buffer to store the path, up to 1024 characters
    char path[1024]; 

    // Path to your symbolic link
    const char* symlinkPath = "/proc/self/exe";
    
    // Read the contents of the symbolic link into the path buffer
    ssize_t pathLength = readlink(symlinkPath, path, sizeof(path) - 1);

    // convert path buffer to string
    std::string str(path, pathLength);

    // find the index of the last slash
    size_t lastSlashIndex = str.find_last_of('/') + 1;

    // replace the space_invaders exe with highscores.txt
    str.replace(lastSlashIndex, pathLength - lastSlashIndex, "highscores.txt");


    // if (length != -1) {
    //     std::cout << "Symbolic link points to: " << filePath << std::endl;
    // } else {
    //     std::cerr << "Error reading symbolic link" << std::endl;    
    // }

    return str;
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

    this->score = score;
    userEntry = "AAA";
    entryIdx = 0;
    tickCnt = 0;
    tickCntMax = 50;
    showChar = true;
    nameEntryY = 0;
    filePath = getPathToHighScoreTxt();
    highScores = readHighScores(filePath);

}

// Used to draw the name entry screen.
void HighScores::drawUserEntry(std::string entry) {
    graphics.drawStrCentered(entry, USER_ENTRY_Y, USER_ENTRY_SIZE, textColor);
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
    uint16_t yPos = 120;
    uint16_t yInc = 20;

    // loop through the vector
    for (const auto& pair : highScores) {
        // add 0 paddings to integer
        std::stringstream ss;
        ss << std::setw(5) << std::setfill('0') << pair.second;

        // combine name and score into one string
        std::string entry = pair.first + "   " + ss.str();
        
        // print to screen
        graphics.drawStrCentered(entry, yPos, USER_ENTRY_SIZE, textColor);

        // increment y position
        yPos += yInc;
    }
}
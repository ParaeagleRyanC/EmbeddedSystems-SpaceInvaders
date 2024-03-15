#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>

#include "PlaySound.h"


PlaySound::PlaySound() {
    getInvaderDieAudio();
    getWalkOneAudio();
    getWalkTwoAudio();
    getWalkThreeAudio();
    getWalkFourAudio();
    getLaserAudio();
    getPlayererDieAudio();
    getUfoDieAudio();
    getUfoAudio();
    fd = open(audioDeviceFilePath, O_RDWR);
    isUfoPlaying = false;
    isTankAlive = true;
}

void PlaySound::playInvaderDie() {
    int isPlaying = 1;
    if (isUfoPlaying) {
        loopOff();
        write(fd, invaderDieBuffer, invaderDieBufferIndex * 4);
        while (isPlaying) read(fd, &isPlaying, 1);
        loopOn();
        playUfo();
    }
    else write(fd, invaderDieBuffer, invaderDieBufferIndex * 4);
}

void PlaySound::playWalkOne() {
    if (!isUfoPlaying) write(fd, walkOneBuffer, walkOneBufferIndex * 4);
}

void PlaySound::playWalkTwo() {
    if (!isUfoPlaying) write(fd, walkTwoBuffer, walkTwoBufferIndex * 4);
}

void PlaySound::playWalkThree() {
    if (!isUfoPlaying) write(fd, walkThreeBuffer, walkThreeBufferIndex * 4);
}

void PlaySound::playWalkFour() {
    if (!isUfoPlaying) write(fd, walkFourBuffer, walkFourBufferIndex * 4);
}

void PlaySound::playLaser() {
    int isPlaying = 1;
    if (isUfoPlaying) {
        loopOff();
        write(fd, laserBuffer, laserBufferIndex * 4);
        while (isPlaying) read(fd, &isPlaying, 1);
        loopOn();
        playUfo();
    }
    else write(fd, laserBuffer, laserBufferIndex * 4);
}

void PlaySound::playUfo() {
    write(fd, ufoBuffer, ufoBufferIndex * 4);
}

void PlaySound::playUfoDie() {
    write(fd, ufoDieBuffer, ufoDieBufferIndex * 4);
}

void PlaySound::playPlayerDie() {
    int isPlaying = 1;
    if (isUfoPlaying) {
        loopOff();
        write(fd, playerDieBuffer, playerDieBufferIndex * 4);
        while (isPlaying) read(fd, &isPlaying, 1);
    }
    else write(fd, playerDieBuffer, playerDieBufferIndex * 4);
}

// helper function to get the filepath to one of the wav files
std::string PlaySound::getPathToWaveFile(std::string name) {
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

    // get actual path
    std::string wavFilePath = "userspace/apps/space_invaders/wavFiles/" + name + ".wav";
    fpath.replace(index, pathLength - index, wavFilePath);

    return fpath;
}


void PlaySound::getInvaderDieAudio() {
    int audioFile = open(getPathToWaveFile(INVADER_DIE).c_str(), O_RDONLY);

    invaderDieBufferIndex = 0;
    int bytesRead = 0;
    int16_t audioData;
    do {
        bytesRead = read(audioFile, &audioData, 2);
        invaderDieBuffer[invaderDieBufferIndex] = audioData << 8; // left shit by 8 to make it 24-bit
        invaderDieBufferIndex++;
    } while (bytesRead > 0);

    close(audioFile);
}

void PlaySound::getWalkOneAudio() {
    int audioFile = open(getPathToWaveFile(WALK_ONE).c_str(), O_RDONLY);

    walkOneBufferIndex = 0;
    int bytesRead = 0;
    int16_t audioData;
    do {
        bytesRead = read(audioFile, &audioData, 2);
        walkOneBuffer[walkOneBufferIndex] = audioData << 8; // left shit by 8 to make it 24-bit
        walkOneBufferIndex++;
    } while (bytesRead > 0);

    close(audioFile);
}

void PlaySound::getWalkTwoAudio() {
    int audioFile = open(getPathToWaveFile(WALK_TWO).c_str(), O_RDONLY);

    walkTwoBufferIndex = 0;
    int bytesRead = 0;
    int16_t audioData;
    do {
        bytesRead = read(audioFile, &audioData, 2);
        walkTwoBuffer[walkTwoBufferIndex] = audioData << 8; // left shit by 8 to make it 24-bit
        walkTwoBufferIndex++;
    } while (bytesRead > 0);

    close(audioFile);
}

void PlaySound::getWalkThreeAudio() {
    int audioFile = open(getPathToWaveFile(WALK_THREE).c_str(), O_RDONLY);

    walkThreeBufferIndex = 0;
    int bytesRead = 0;
    int16_t audioData;
    do {
        bytesRead = read(audioFile, &audioData, 2);
        walkThreeBuffer[walkThreeBufferIndex] = audioData << 8; // left shit by 8 to make it 24-bit
        walkThreeBufferIndex++;
    } while (bytesRead > 0);

    close(audioFile);
}

void PlaySound::getWalkFourAudio() {
    int audioFile = open(getPathToWaveFile(WALK_FOUR).c_str(), O_RDONLY);

    walkFourBufferIndex = 0;
    int bytesRead = 0;
    int16_t audioData;
    do {
        bytesRead = read(audioFile, &audioData, 2);
        walkFourBuffer[walkFourBufferIndex] = audioData << 8; // left shit by 8 to make it 24-bit
        walkFourBufferIndex++;
    } while (bytesRead > 0);

    close(audioFile);
}

void PlaySound::getLaserAudio() {
    int audioFile = open(getPathToWaveFile(LASER).c_str(), O_RDONLY);

    laserBufferIndex = 0;
    int bytesRead = 0;
    int16_t audioData;
    do {
        bytesRead = read(audioFile, &audioData, 2);
        laserBuffer[laserBufferIndex] = audioData << 8; // left shit by 8 to make it 24-bit
        laserBufferIndex++;
    } while (bytesRead > 0);

    close(audioFile);
}

void PlaySound::getPlayererDieAudio() {
    int audioFile = open(getPathToWaveFile(PLAYER_DIE).c_str(), O_RDONLY);

    playerDieBufferIndex = 0;
    int bytesRead = 0;
    int16_t audioData;
    do {
        bytesRead = read(audioFile, &audioData, 2);
        playerDieBuffer[playerDieBufferIndex] = audioData << 8; // left shit by 8 to make it 24-bit
        playerDieBufferIndex++;
    } while (bytesRead > 0);

    close(audioFile);
}

void PlaySound::getUfoDieAudio() {
    int audioFile = open(getPathToWaveFile(UFO_DIE).c_str(), O_RDONLY);

    ufoDieBufferIndex = 0;
    int bytesRead = 0;
    int16_t audioData;
    do {
        bytesRead = read(audioFile, &audioData, 2);
        ufoDieBuffer[ufoDieBufferIndex] = audioData << 8; // left shit by 8 to make it 24-bit
        ufoDieBufferIndex++;
    } while (bytesRead > 0);

    close(audioFile);
}

void PlaySound::getUfoAudio() {
    int audioFile = open(getPathToWaveFile(UFO_).c_str(), O_RDONLY);

    ufoBufferIndex = 0;
    int bytesRead = 0;
    int16_t audioData;
    do {
        bytesRead = read(audioFile, &audioData, 2);
        ufoBuffer[ufoBufferIndex] = audioData << 8; // left shit by 8 to make it 24-bit
        ufoBufferIndex++;
    } while (bytesRead > 0);

    close(audioFile);
}

void PlaySound::loopOn() { ioctl(fd, LOOP_ON); }

void PlaySound::loopOff() { ioctl(fd, LOOP_OFF); }

void PlaySound::setUfoPlaying() { isUfoPlaying = !isUfoPlaying; }

void PlaySound::setTankAlive() { isTankAlive = !isTankAlive; }
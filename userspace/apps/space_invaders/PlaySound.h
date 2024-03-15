#ifndef PLAY_SOUND_H
#define PLAY_SOUND_H

#include <cstdint>
#include <string>
#include <fcntl.h>

#include "audio_config/audio_config.h"

#define BUFFER_SIZE 128000 //(512 / 4) * 1024
#define INVADER_DIE "invader_die"
#define LASER "laser_fixed"
#define PLAYER_DIE "player_die"
#define UFO_ "ufo"
#define UFO_DIE "ufo_die"
#define WALK_ONE "walk1"
#define WALK_TWO "walk2"
#define WALK_THREE "walk3"
#define WALK_FOUR "walk4"
#define LOOP_ON _IOW('u', 1, int)
#define LOOP_OFF _IOW('u', 2, int)


class PlaySound {
    public:
        PlaySound();

    private:
        int32_t invaderDieBuffer[BUFFER_SIZE];
        int32_t walkOneBuffer[BUFFER_SIZE];
        int32_t walkTwoBuffer[BUFFER_SIZE];
        int32_t walkThreeBuffer[BUFFER_SIZE];
        int32_t walkFourBuffer[BUFFER_SIZE];
        int32_t laserBuffer[BUFFER_SIZE];
        int32_t ufoBuffer[BUFFER_SIZE];
        int32_t ufoDieBuffer[BUFFER_SIZE];
        int32_t playerDieBuffer[BUFFER_SIZE];

        int invaderDieBufferIndex;
        int walkOneBufferIndex;
        int walkTwoBufferIndex;
        int walkThreeBufferIndex;
        int walkFourBufferIndex;
        int laserBufferIndex;
        int ufoBufferIndex;
        int ufoDieBufferIndex;
        int playerDieBufferIndex;

        bool isUfoPlaying;

        void getInvaderDieAudio();
        void getWalkOneAudio();
        void getWalkTwoAudio();
        void getWalkThreeAudio();
        void getWalkFourAudio();
        void getLaserAudio();
        void getPlayererDieAudio();
        void getUfoDieAudio();
        void getUfoAudio();

        std::string getPathToWaveFile(std::string name);
        const char* audioDeviceFilePath = "/dev/audio_codec"; 
        int fd;

    public:
        bool isTankAlive;
        void playInvaderDie();
        void playWalkOne();
        void playWalkTwo();
        void playWalkThree();
        void playWalkFour();
        void playLaser();
        void playUfo();
        void playUfoDie();
        void playPlayerDie();
        void loopOn();
        void loopOff();
        void setUfoPlaying();
        void setTankAlive();
};

#endif /* PLAY_SOUND_H */

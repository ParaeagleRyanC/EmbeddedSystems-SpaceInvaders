
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "audio_config/audio_config.h"
#define BUFFER_SIZE 128000 //(512 / 4) * 1024

const char* deviceFilePath = "/dev/audio_codec"; 
int32_t audioBuffer[BUFFER_SIZE];
int16_t audioData;
int is_playing = 1;

int main(int argc, char *argv[]) {
    
    // Check if a file path is provided as a command line argument
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file_path>\n", argv[0]);
        return 1; // Exit with an error code
    }

    // Get the file path from the command line arguments
    const char *filePath = argv[1];

    // Open the file
    int audioFile = open(filePath, O_RDONLY);

    // Check if the file is successfully opened
    if (audioFile < 0) {
        fprintf(stderr, "Error opening file: %s\n", filePath);
        return 1; // Exit with an error code
    }

    // Initialize the audio codec chip via I2C
    audio_config_init();

    // set volume
    audio_config_set_volume(60);

    int bytesRead = 0;
    int bufferIndex = 0;
    do {
        bytesRead = read(audioFile, &audioData, 2);
        audioBuffer[bufferIndex] = audioData << 8; // left shit by 8 to make it 24-bit
        bufferIndex++;
    } while (bytesRead > 0);
    printf("Read %d samples\n", bufferIndex);
    
    int fd = open(deviceFilePath, O_RDWR);
    if(fd < 0){
        printf("Error opening audio device \n");
        return -1;
    }

    // play audio twice 
    for (int i = 0; i < 2; i++) {
        printf("Write\n");
        write(fd, audioBuffer, bufferIndex * 4);
        printf("Write complete\n");
        // wait for the sound to finish playing
        while (is_playing) read(fd, &is_playing, 1);
    }

    // Close the file
    close(audioFile);
    close(fd);

    return 0; // Exit successfully
}
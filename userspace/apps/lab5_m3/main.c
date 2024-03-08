
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "audio_config/audio_config.h"
#define BUFFER_SIZE 512 / 4 * 1024


int32_t audioBuffer[BUFFER_SIZE];
int16_t audioData;

int main(int argc, char *argv[]) {

    
    // Check if a file path is provided as a command line argument
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file_path>\n", argv[0]);
        return 1; // Exit with an error code
    }

    // Get the file path from the command line arguments
    const char *filePath = argv[1];

    // Open the file
    int file = fopen(filePath, "r");

    // Check if the file is successfully opened
    if (inputFile == NULL) {
        fprintf(stderr, "Error opening file: %s\n", filePath);
        return 1; // Exit with an error code
    }

    int bytesRead = 0;
    int bufferIndex = 0;
    do {
        bytesRead = read(file, &audioData, 2);
        audioBuffer[bufferIndex] = bytesRead << 8; // left shit by 8 to make it 24-bit
        bufferIndex++;
    } while (bytesRead > 0);

    

    // Initialize the audio codec chip via I2C
    audio_config_init();








    // Close the file
    fclose(inputFile);

    return 0; // Exit successfully
}
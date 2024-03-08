
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "audio_config/audio_config.h"

int main(int argc, char *argv[]) {
    
    // Check if a file path is provided as a command line argument
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file_path>\n", argv[0]);
        return 1; // Exit with an error code
    }

    // Get the file path from the command line arguments
    const char *filePath = argv[1];

    // Open the file
    FILE *inputFile = fopen(filePath, "r");

    // Check if the file is successfully opened
    if (inputFile == NULL) {
        fprintf(stderr, "Error opening file: %s\n", filePath);
        return 1; // Exit with an error code
    }

    // Read and process the contents of the file (you can replace this part with your logic)
    printf("File contents:\n");
    char line[1024];
    while (fgets(line, sizeof(line), inputFile) != NULL) {
        printf("%s", line);
    }

    // Initialize the audio codec chip via I2C
    audio_config_init();








    // Close the file
    fclose(inputFile);

    return 0; // Exit successfully
}
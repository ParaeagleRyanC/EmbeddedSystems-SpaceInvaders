// copied from chatGPT with prompt 
// "write a function that calls read and write functions from a given device file in C"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

void readWriteToDevice(const char* deviceFilePath) {
    // Open the device file for reading and writing
    int fd = open(deviceFilePath, O_RDWR);

    if (fd == -1) {
        perror("Error opening the device file");
        exit(EXIT_FAILURE);
    }

    // Example buffer for reading and writing
    char buffer[1024];

    // Reading from the device
    ssize_t bytesRead = read(fd, buffer, sizeof(buffer));

    if (bytesRead == -1) {
        perror("Error reading from the device");
        close(fd);
        exit(EXIT_FAILURE);
    }

    printf("Read %zd bytes from the device:\n%s\n", bytesRead, buffer);

    // Writing to the device
    const char* dataToWrite = "Hello, Device!";
    ssize_t bytesWritten = write(fd, dataToWrite, strlen(dataToWrite));

    if (bytesWritten == -1) {
        perror("Error writing to the device");
        close(fd);
        exit(EXIT_FAILURE);
    }

    printf("Wrote %zd bytes to the device.\n", bytesWritten);

    // Close the device file
    close(fd);
}

int main() {
    const char* deviceFilePath = "/dev/audio_codec"; 

    readWriteToDevice(deviceFilePath);

    return 0;
}
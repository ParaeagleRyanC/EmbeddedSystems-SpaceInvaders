
#include <stdbool.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include "switches.h"

#define UIO_SWITCHES_MMAP_SIZE 0x00010000        // size of memory to allocate
#define OPEN_ERROR -1                   // open() returns -1 on error
#define DATA_REGISTER_OFFSET 0x0000
#define INTERRUPT_ENABLE_REG_OFFSET 0x0128
#define GLOBAL_INTERRUPT_ENABLE_REG_OFFSET 0x011C
#define INTERRUPT_STATUS_REG 0x0120
#define GLOBAL_INTERRUPT_LEFTSHIT_OFFSET 31

/********************************** globals ***********************************/
static int f;     // this is a file descriptor that describes an open UIO device
static char *ptr; // this is the virtual address of the UIO device registers

// Initialize the driver
//  devFilePath: The file path to the uio dev file
//  Return: An error code on error, SWITCHES_SUCCESS otherwise
// This must be called before calling any other switches_* functions
//
//  Tip: This function won't be able to open the UIO device file unless the
//  program is run with ''sudo''.  This is easy to forget, so it is helpful to
//  code an error message into this function that says "Did you forget to
//  sudo?", if it cannot open the UIO file.
int32_t switches_init(const char *devFilePath) {
    // open the device
    f = open(devFilePath, O_RDWR);
    if (f == OPEN_ERROR) {
        printf("Init error -- did you forget to sudo?\n");
        return OPEN_ERROR;
    }

    // memory map the physical address of the hardware into virtual address space
    ptr = mmap(NULL, UIO_SWITCHES_MMAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, 
                f, DATA_REGISTER_OFFSET);
    if (ptr == MAP_FAILED) {
        return OPEN_ERROR;
    }

    return SWITCHES_SUCCESS;
}

// Return the current state of the switches
uint8_t switches_read() {
    return *((volatile uint32_t *)(ptr + DATA_REGISTER_OFFSET));
}

// Call this on exit to clean up
void switches_exit() {
    munmap(ptr, UIO_SWITCHES_MMAP_SIZE);
    close(f);
}

// Enable GPIO interrupt output
void switches_enable_interrupts() {
    *((volatile uint32_t *)(ptr + INTERRUPT_ENABLE_REG_OFFSET)) = 0x1;
    *((volatile uint32_t *)(ptr + GLOBAL_INTERRUPT_ENABLE_REG_OFFSET)) = 0x1 << GLOBAL_INTERRUPT_LEFTSHIT_OFFSET;
}

// Disable GPIO interrupt output
void switches_disable_interrupts() {
    *((volatile uint32_t *)(ptr + INTERRUPT_ENABLE_REG_OFFSET)) = 0x0;
    *((volatile uint32_t *)(ptr + GLOBAL_INTERRUPT_ENABLE_REG_OFFSET)) = 0x0 << GLOBAL_INTERRUPT_LEFTSHIT_OFFSET;
}

// Return whether an interrupt is pending
bool switches_interrupt_pending() {
    return *((volatile uint32_t *)(ptr + INTERRUPT_STATUS_REG)) != 0;
}

// Acknowledge a pending interrupt
void switches_ack_interrupt() {
    *((volatile uint32_t *)(ptr + INTERRUPT_STATUS_REG)) = 0x1;
}

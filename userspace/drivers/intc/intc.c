
#include <stdint.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include "intc.h"

#define DATA_REGISTER_OFFSET 0x0000
#define UIO_INTERRUPT_MMAP_SIZE 0x00010000        // size of memory to allocate
#define OPEN_ERROR -1                   // open() returns -1 on error
#define ISR_OFFSET 0x00
#define IER_OFFSET 0x08
#define IAR_OFFSET 0x0C
#define MER_OFFSET 0x1C
#define MER_SET_HIE_ME_HIGH 0x3

/********************************** globals ***********************************/
static int f;     // this is a file descriptor that describes an open UIO device
static char *ptr; // this is the virtual address of the UIO device registers


// function to enable MER
void enable_mer() {
    *((volatile uint32_t *)(ptr + MER_OFFSET)) = MER_SET_HIE_ME_HIGH;
}

// Initializes the driver (opens UIO file and calls mmap)
// devDevice: The file path to the uio dev file
// Returns: A negative error code on error, INTC_SUCCESS otherwise
// This must be called before calling any other intc_* functions
//
//  Tip: This function won't be able to open the UIO device file unless the
//  program is run with ''sudo''.  This is easy to forget, so it is helpful to
//  code an error message into this function that says "Did you forget to
//  sudo?", if it cannot open the UIO file.
int32_t intc_init(const char devDevice[]) {
    // open the device
    f = open(devDevice, O_RDWR);
    if (f == OPEN_ERROR) {
        printf("Init error -- did you forget to sudo?\n");
        return OPEN_ERROR;
    }

    // memory map the physical address of the hardware into virtual address space
    ptr = mmap(NULL, UIO_INTERRUPT_MMAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, 
                f, DATA_REGISTER_OFFSET);
    if (ptr == MAP_FAILED) {
        return OPEN_ERROR;
    }
    
    intc_enable_uio_interrupts();
    enable_mer();

    return INTC_SUCCESS;
}

// Called to exit the driver (unmap and close UIO file)
void intc_exit() {
    munmap(ptr, UIO_INTERRUPT_MMAP_SIZE);
    close(f);
}

// This function will block until an interrupt occurrs
// Returns: Bitmask of activated interrupts
uint32_t intc_wait_for_interrupt() {
    uint32_t activated_interrupts = 0;
    uint32_t bitmask_activated_interrupts = 0;
    read(f, &activated_interrupts, sizeof(activated_interrupts));
    bitmask_activated_interrupts = *((volatile uint32_t *)(ptr + ISR_OFFSET));
    return bitmask_activated_interrupts;
}

// Acknowledge interrupt(s) in the interrupt controller
// irq_mask: Bitmask of interrupt lines to acknowledge.
void intc_ack_interrupt(uint32_t irq_mask) {
    *((volatile uint32_t *)(ptr + IAR_OFFSET)) = irq_mask;
}

// // Instruct the UIO to enable interrupts for this device in Linux
// // (see the UIO documentation for how to do this)
void intc_enable_uio_interrupts() {
    uint32_t enable = 1;
    int32_t bytes_written = write(f, &enable, sizeof(enable));
    if (bytes_written == -1) {
        printf("Error enabling uio interrupts.");
    }
}

// Enable interrupt line(s)
// irq_mask: Bitmask of lines to enable
// This function only enables interrupt lines, ie, a 0 bit in irq_mask
//	will not disable the interrupt line
void intc_irq_enable(uint32_t irq_mask) {
    *((volatile uint32_t *)(ptr + IER_OFFSET)) |= irq_mask;
}

// Same as intc_irq_enable, except this disables interrupt lines
void intc_irq_disable(uint32_t irq_mask) {
    *((volatile uint32_t *)(ptr + IER_OFFSET)) &= irq_mask;
}

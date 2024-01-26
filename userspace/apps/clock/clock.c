#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "system.h"
#include "intc/intc.h"
#include "buttons/buttons.h"
#include "switches/switches.h"

#define EXIT_ERROR -1
#define DEBOUNCED_MS 30
#define TEN_MS 10
#define ONE_SECOND_MS 1000
#define SECOND_ROLLOVER 59
#define MINUTE_ROLLOVER 59
#define HOUR_ROLLOVER 23

uint8_t buttons;
uint8_t switches;
int hour = 23;
int minute = 58;
int second = 50;
uint16_t debounceTimer = 0;
uint16_t secondTimer = 0;
bool increment;


void print_time() {
  printf("\033[2J\033[H");
  printf("%02d:%02d:%02d", hour, minute, second);
}

void regular_clock_advance() {
  if (secondTimer >= ONE_SECOND_MS) {
    if (second == SECOND_ROLLOVER) {
      second = 0;
      if (minute == MINUTE_ROLLOVER) {
        minute = 0;
        if (hour == HOUR_ROLLOVER) {
          hour = 0;
        }
        else hour++;
      }
      else minute++;
    }
    else second++;
    secondTimer = 0;
  }
}

void inc_dec_second() {
  if (increment) {
    if (second == SECOND_ROLLOVER) {
      second = 0;
      if (minute == MINUTE_ROLLOVER) {
        minute = 0;
        if (hour == HOUR_ROLLOVER) hour = 0;
        else hour++;
      }
      else minute++;
    }
    else second++;
  }
  else {
    if (second == 0) {
      second = SECOND_ROLLOVER;
      if (minute == 0) {
        minute = MINUTE_ROLLOVER;
        if (hour == 0) {
          hour = HOUR_ROLLOVER; 
        }
        else hour--;
      }
      else minute--;
    }
    else second--;
  }
}

// This is invoked in response to a timer interrupt.
// It does 2 things: 1) help debounce buttons, and 2) advances the time.
void isr_fit() {
  // read switches value
  switches = switches_read();
  
  // if SW1 is high, clock stops
  if (switches == SWITCHES_1_MASK) return;

  // if SW0 is high, increment flag is set to high
  if (switches == SWITCHES_0_MASK) increment = true;
  else increment = false;

  // increment second timer 
  secondTimer += TEN_MS;

  // increment debounce timer
  debounceTimer += TEN_MS;

  // button debounced, proceed to increment/decrement
  if (debounceTimer >= DEBOUNCED_MS) {
    // reset secondTimer so regular_clock_advance() doesn't get executed
    secondTimer = 0;
    if (buttons == BUTTONS_0_MASK) inc_dec_second();
  }

  // advance the clock as normal
  regular_clock_advance();

  print_time();
}

// This is invoked each time there is a change in the button state (result of a
// push or a bounce).
void isr_buttons() {
  // Read the state of the buttons
  buttons = buttons_read();

  // ... do something ...
  debounceTimer = 0;

  // Acknowledge the button interrupt
  buttons_ack_interrupt();
}

// Run the clock application
int main() {
  int32_t err;

  // Initialize interrupt controller driver
  err = intc_init(SYSTEM_INTC_UIO_FILE);
  if (err) {
    printf("intc_init failed\n");
    exit(EXIT_ERROR);
  }

  // Initialize buttons
  err = buttons_init(SYSTEM_BUTTONS_UIO_FILE);
  if (err) {
    printf("buttons_init failed\n");
    exit(EXIT_ERROR);
  }

  // Initialize switches
  err = switches_init(SYSTEM_SWITCHES_UIO_FILE);
  if (err) {
    printf("switches_init failed\n");
    exit(EXIT_ERROR);
  }

  // Enable interrupt output from buttons
  buttons_enable_interrupts();

  // Enable button and FIT interrupt lines on interrupt controller
  intc_irq_enable(SYSTEM_INTC_IRQ_BUTTONS_MASK);
  intc_irq_enable(SYSTEM_INTC_IRQ_FIT_MASK);

  while (1) {
    // Call interrupt controller function to wait for interrupt
    uint32_t interrupts = intc_wait_for_interrupt();

    // Check which interrupt lines are high and call the appropriate ISR
    // functions
    if (interrupts & SYSTEM_INTC_IRQ_FIT_MASK)
      isr_fit();
    if (interrupts & SYSTEM_INTC_IRQ_BUTTONS_MASK)
      isr_buttons();

    // Acknowledge the intc interrupt
    intc_ack_interrupt(interrupts);

    // Re-enable UIO interrupts
    intc_enable_uio_interrupts();
  }

  intc_exit();
  buttons_exit();

  return 0;
}

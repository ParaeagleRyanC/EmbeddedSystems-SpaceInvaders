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
#define TENTH_SECOND_MS 100
#define HALF_SECOND_MS 500
#define ONE_SECOND_MS 1000
#define SECOND_ROLLOVER 59
#define MINUTE_ROLLOVER 59
#define HOUR_ROLLOVER 23
#define ALL_BUTTONS 0x3

static uint8_t buttons;
static uint8_t switches;
static uint16_t hour = 23;
static uint16_t minute = 58;
static uint16_t second = 50;
static uint16_t debounceTimer = 0;
static uint16_t secondTimer = 0;
static uint16_t fastIncDecTimer = 0;
static bool increment;

// print current time 
void print_time() {
  printf("\r%02d:%02d:%02d", hour, minute, second);
  fflush(stdout);
}

// increment the clock as normal
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

// increment or decrement second
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

// increment or decrement minute
void inc_dec_minute() {
  if (increment) {
    if (minute == MINUTE_ROLLOVER) {
      minute = 0;
      if (hour == HOUR_ROLLOVER) {
        hour = 0;
      }
      else hour++;
    }
    else minute++;
  }
  else {
    if (minute == 0) {
      minute = MINUTE_ROLLOVER;
      if (hour == 0) {
        hour = HOUR_ROLLOVER;
      }
      else hour--;
    }
    else minute--;
  }
}

// increment or decrement hour
void inc_dec_hour() {
  if (increment) {
    if (hour == HOUR_ROLLOVER) hour = 0;
    else hour++;
  }
  else {
    if (hour == 0) hour = HOUR_ROLLOVER;
    else hour--;
  }
}

// This is invoked in response to a timer interrupt.
// It does 2 things: 1) help debounce buttons, and 2) advances the time.
void isr_fit() {
  // read switches value
  switches = switches_read();
  
  // if SW1 is high, clock stops
  if (switches & SWITCHES_1_MASK) {
    // advance the clock as normal
    regular_clock_advance();
  };

  // if SW0 is high, increment flag is set to high
  if (switches & SWITCHES_0_MASK) increment = true;
  else increment = false;

  // increment second timer 
  secondTimer += TEN_MS;

  // increment debounce timer
  debounceTimer += TEN_MS;

  // button debounced, proceed to increment/decrement
  if (debounceTimer == DEBOUNCED_MS) {
    // inc/dec base on which button is pushed
    if (buttons == BUTTONS_0_MASK) inc_dec_second();
    else if (buttons == BUTTONS_1_MASK) inc_dec_minute();
    else if (buttons == BUTTONS_2_MASK) inc_dec_hour();
    // default to inc/dec second if multiple buttons pushed at once
    else if (buttons & ALL_BUTTONS) inc_dec_second();
  }

  // increment/decrement fast
  if (debounceTimer >= HALF_SECOND_MS && buttons) {
    fastIncDecTimer += TEN_MS;
    if (fastIncDecTimer >= TENTH_SECOND_MS) {
      // inc/dec base on which button is pushed
      if (buttons == BUTTONS_0_MASK) inc_dec_second();
      else if (buttons == BUTTONS_1_MASK) inc_dec_minute();
      else if (buttons == BUTTONS_2_MASK) inc_dec_hour();
      // default to inc/dec second if multiple buttons pushed at once
      else if (buttons & ALL_BUTTONS) inc_dec_second();
      fastIncDecTimer = 0;
    }
  }

  // print time 
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

#include "bug/panic.h"
#include "bug/debug.h"
#include <avr/interrupt.h>

static inline void wait() {
  for (volatile long i = 0; i < 20000; ++i) {};
}

void panic() {
  cli();
  while(1) {
    for(int i = 0; i < DEBUG_LEDS; ++i) {
      debug_led((i - 1) % DEBUG_LEDS, 0);
      debug_led(i, 1);
      wait();
    }
    for(int i = DEBUG_LEDS - 1; i >= 0; --i) {
      debug_led((i + 1) % DEBUG_LEDS, 0);
      debug_led(i, 1);
      wait();
    }
  }
}
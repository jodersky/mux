#include <avr/interrupt.h>
#include "mux/clock.h"
#include "mux/sched.h"
#include "mcu/context.h"

static void (*on_tick)();

void clock_init(int ms, void (*tick)()) {
  TCCR3A = 0;
  TCCR3B = 0;
  TCCR3C = 0;

  TCCR3B = (1 << WGM32); // turn on CTC mode:
  TCCR3B |= (1 << CS32) | (0 << CS31) | (1 << CS30); // set to 1024 prescaler

  unsigned long int fcpu = (unsigned long int) F_CPU;
  unsigned long int hz = 1000l / ((unsigned long int) ms);

  unsigned long int hz_counter = fcpu / (1024l * (hz)) - 1;
  OCR3A = hz_counter;
  on_tick = tick;
}

void clock_start() {
  TIMSK3 |= (1 << OCIE3A);
}

void clock_stop() {
  TIMSK3 &= ~(1 << OCIE3A);
}

ISR(TIMER3_COMPA_vect, ISR_NAKED) {
  context_save();
  on_tick();
  context_restore();
  reti();
}

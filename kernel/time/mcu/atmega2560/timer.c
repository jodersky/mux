#include <avr/interrupt.h>
#include "sched/sched.h"
#include "time/timer.h"

void timer_init() {
  cli();
  TCCR3A = 0;
  TCCR3B = 0;
  TCCR3C = 0;

  TCCR3B = (1 << WGM32); // turn on CTC mode:
  TCCR3B |= (1 << CS32) | (0 << CS31) | (1 << CS30); // set to 1024 prescaler

  unsigned int hz_counter = F_CPU / (2 * 1024 * HZ) - 1;
  OCR3A = hz_counter;
  sei();
}

void timer_start() {
  TIMSK3 |= (1 << OCIE3A);
}

void timer_stop() {
  TIMSK3 &= ~(1 << OCIE3A);
}

ISR(TIMER3_COMPA_vect, ISR_NAKED) {
  SAVE_CONTEXT();
  sched_tick();
  sei();
  RESTORE_CONTEXT();
  asm volatile ("reti");
}

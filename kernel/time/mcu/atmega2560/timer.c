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

  unsigned long int hz_counter = ((unsigned long int) F_CPU) / (1024 * ((unsigned long int) HZ)) - 1;
  OCR3A = hz_counter;
  sei();
}

void timer_start() {
  TIMSK3 |= (1 << OCIE3A);
}

void timer_stop() {
  TIMSK3 &= ~(1 << OCIE3A);
}

#include <avr/io.h>
void led13(int on) {
  if (on) {
    DDRB |= (1 << 7);
    PORTB |= (1 << 7);
  } else {
    DDRB &= ~(1 << 7);
    PORTB &= ~(1 << 7);
  }
}

ISR(TIMER3_COMPA_vect, ISR_NAKED) {
  SAVE_CONTEXT();
  static int on = 0;
  on = !on;
  led13(on);
  sched_tick();
  RESTORE_CONTEXT();
  asm volatile ("reti");
}

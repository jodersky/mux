#include <avr/power.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include "mux/idle.h"
#include "mux/sched.h"


void idle_entry(char args) {
  while(1) {
    set_sleep_mode(SLEEP_MODE_IDLE);
    cli();
    sleep_enable();
    sei();
    sleep_cpu();
    sleep_disable();
  };
}


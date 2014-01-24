#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include "sched/idle.h"
#include "tshield/tshield.h"

void idle_entry(char args) {
  while(1) {
    tshield_led(TSHIELD_LED_IDLE,1);
    set_sleep_mode(SLEEP_MODE_IDLE);
    cli();
    sleep_enable();
    sei();
    sleep_cpu();
    sleep_disable();
  };
}


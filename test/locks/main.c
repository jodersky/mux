#include <stddef.h>
#include <stdio.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "task/task.h"
#include "task/sched.h"
#include "bug/panic.h"
#include "bug/debug.h"
#include "task/lock.h"
#include "time/clock.h"
#include "mcu/task/context.h"
#include "tshield/tshield.h"

#define WAIT_CYCLES(cycles) for (volatile unsigned long i = 0; i < cycles; ++i) {}

spin_lock_t on_lock = SPIN_LOCK_UNLOCKED;
volatile char on = 0;

void read(char id) {
  while(1) {
    spin_lock(&on_lock);
    debug_led(0,on);
    spin_unlock(&on_lock);
  }
}

void write( char id) {
  while(1) {
    spin_lock(&on_lock);
    on = !on;
    WAIT_CYCLES(30000);
    spin_unlock(&on_lock);
  }
}


DECLARE_TASK(task1, DEFAULT_STACK_SIZE, read, 1);
DECLARE_TASK(task2, DEFAULT_STACK_SIZE, write, 2);


int main(int argc, char *argv[]) {
  cli();
  tshield_init();
  

  spawn(&task1);
  spawn(&task2);
  
  sei();
  clock_init(10, schedule);
  clock_start();
  sched_init();
  panic(); //should never reach here
  while(1){}
}

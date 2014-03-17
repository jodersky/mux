#include <stddef.h>
#include <stdio.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "task/task.h"
#include "task/sched.h"
#include "bug/panic.h"
#include "bug/debug.h"
#include "mcu/task/context.h"
#include "tshield/tshield.h"

#define WAIT_CYCLES(cycles) for (volatile unsigned long i = 0; i < cycles; ++i) {}

struct list_head frozen = LIST_HEAD_INIT(frozen);

void freeze() __attribute__ ( ( naked ) );
void freeze() {
  context_save();
  sleep_queue(&frozen);
  schedule();
  context_restore();
}

void blink( char id) {
  while(1) {
    debug_led(id - 1,1);
    WAIT_CYCLES((long) 30000);
    debug_led(id - 1,0);
    WAIT_CYCLES((long) 30000);
  }
}


DECLARE_TASK(task1, DEFAULT_STACK_SIZE, blink, 1);
DECLARE_TASK(task2, DEFAULT_STACK_SIZE, blink, 2);
DECLARE_TASK(task3, DEFAULT_STACK_SIZE, blink, 3);
DECLARE_TASK(task4, DEFAULT_STACK_SIZE, blink, 4);


int main(int argc, char *argv[]) {
  cli();
  tshield_init();

  

  spawn(&task1);
  spawn(&task2);
  spawn(&task3);
  spawn(&task4);
  
  sei();
  clock_init(10, schedule);
  clock_start();
  sched_init();


  panic(); //should never reach here
  while(1){}
}

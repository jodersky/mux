#include <stddef.h>
#include <sched/sched.h>
#include <time/timer.h>
#include <bug/panic.h>
#include <bug/debug.h>
#include <tshield/tshield.h>

#define WAIT_CYCLES(cycles) for (volatile unsigned long i = 0; i < cycles; ++i) {}

void blink( char id) {
  while(1) {
    debug_led(id,1);
    yield();
    WAIT_CYCLES(5000);
    debug_led(id,0);
    yield();
    WAIT_CYCLES(5000);
  }
}

DECLARE_TASK(task1, STACK_SIZE, blink);
DECLARE_TASK(task2, STACK_SIZE, blink);
DECLARE_TASK(task3, STACK_SIZE, blink);


int main(int argc, char *argv[]) {
  cli();
  tshield_init();
  timer_init();

  spawn(&task1, 1);
  spawn(&task2, 2);
  spawn(&task3, 3);

  timer_start();
  sched_init();
  while(1){}
}

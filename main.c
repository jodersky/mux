#include <stddef.h>
#include <sched/sched.h>
#include <time/timer.h>
#include <bug/panic.h>
#include <bug/debug.h>
#include <tshield/tshield.h>
#include <collection/list.h>

#define WAIT_CYCLES(cycles) for (volatile unsigned long i = 0; i < cycles; ++i) {}

static struct list_head frozen = LIST_HEAD_INIT(frozen);
void freeze() __attribute__ ( ( naked ) );
void freeze() {
  cli();
  SAVE_CONTEXT();
  sleep_on(&frozen);
  schedule();
  RESTORE_CONTEXT();
  sei();
  asm volatile ( "ret" );
}

void wake() __attribute__ ( ( naked ) );
void wake() {
  cli();
  SAVE_CONTEXT();
  wake_all(&frozen);
  RESTORE_CONTEXT();
  sei();
  asm volatile ( "ret" );
}


void blink( char id) {
  while(1) {
    debug_led(id,1);
    //yield();
    WAIT_CYCLES(5000);
    debug_led(id,0);
    //yield();
    WAIT_CYCLES(5000);
    freeze();
  }
}

void read(char id) {
  while(1) {
    debug_led(0, 0);
    debug_led(3, 0);
    unsigned char buttons = tshield_read();
    if (buttons != 0) {
      wake();
      debug_led(0, 1);
      unsigned int counter = 0;
      if (counter > 0) {
        debug_led(3, 1);
      }
      if (list_empty(&frozen)) {
        debug_led(3, 1);
      }
    }
  }
}

DECLARE_TASK(task1, STACK_SIZE, blink);
DECLARE_TASK(task2, STACK_SIZE, blink);
DECLARE_TASK(task3, STACK_SIZE, blink);
DECLARE_TASK(task4, STACK_SIZE, read);


int main(int argc, char *argv[]) {
  cli();
  tshield_init();
  timer_init();

  spawn(&task1, 1);
  spawn(&task2, 2);
  spawn(&task3, 3);
  spawn(&task4, 4);

  timer_start();
  sched_init();
  while(1){}
}

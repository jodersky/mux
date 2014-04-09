#include "init.h"
#include "mux/debug.h"
#include "mux/sched.h"
#include "mux/idle.h"
#include "mux/clock.h"
#include "mcu/context.h"

#define HZ 100

DECLARE_TASK(task_idle, IDLE_STACK_SIZE, idle_entry);
DECLARE_TASK(tcb1, DEFAULT_STACK_SIZE, task1);
DECLARE_TASK(tcb2, DEFAULT_STACK_SIZE, task2);

int main(int argc, char *argv[]) {
  cli();

  setup();

  spawn_idle(&task_idle, 0);
  spawn(&tcb1, 1);
  //spawn(&tcb2, 2);

  sei();
  clock_init(1000/HZ, schedule);
  clock_start();
  sched_init();
  panic(); //should never reach here
  while(1){}
}
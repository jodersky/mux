#include <stddef.h>
#include <stdio.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "bug/panic.h"
#include "bug/debug.h"
#include "task/task.h"
#include "task/sched.h"
#include "task/lock.h"
#include "task/idle.h"
#include "time/clock.h"
#include "io/usart.h"
#include "tshield/tshield.h"
#include "mcu/task/context.h"
#include "shell.h"

#define IN_LENGTH 64

#define WAIT_CYCLES(cycles) for (volatile unsigned long i = 0; i < cycles; ++i) {}


DECLARE_TASK(task_idle, IDLE_STACK_SIZE, idle_entry, 0);
DECLARE_TASK(task1, DEFAULT_STACK_SIZE, shell, 1);

int main(int argc, char *argv[]) {
  cli();
  tshield_init();
  usart_init(115200);

  spawn_idle(&task_idle);
  spawn(&task1);

  sei();
  clock_init(10, schedule);
  clock_start();
  sched_init();
  panic(); //should never reach here
  while(1){}
}

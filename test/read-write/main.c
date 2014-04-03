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
#include "io/io.h"
#include "mcu/io/usart.h"
#include "tshield/tshield.h"
#define IN_LENGTH 64

#define WAIT_CYCLES(cycles) for (volatile unsigned long i = 0; i < cycles; ++i) {}

void writer() {

  char* buffer = "hello world";
  size_t length = 11;


  while(1) {
    debug_led(1, 0);
    WAIT_CYCLES(300000);
    debug_led(1, 1);
    write(&usart0, buffer, length);
  }
}

void reader() {
  char in;

  while(1) {
    debug_led(2, 0);
    debug_led(3, 0);
    if (read(&usart0, &in, 1) >= 0) {
      debug_led(2, 1);
    } else {
      debug_led(3, 1);
      WAIT_CYCLES(30000);
    }
    
  }
}

DECLARE_TASK(task_idle, IDLE_STACK_SIZE, idle_entry, 0);
DECLARE_TASK(task1, DEFAULT_STACK_SIZE, writer, 1);
DECLARE_TASK(task2, DEFAULT_STACK_SIZE, reader, 2);

int main(int argc, char *argv[]) {
  cli();
  tshield_init();

  open(&usart0);
  ioctl(&usart0, IOCTL_SET_BAUD, 115200);

  spawn_idle(&task_idle);
  spawn(&task1);
  spawn(&task2);

  sei();
  clock_init(10, schedule);
  clock_start();
  sched_init();
  panic(); //should never reach here
  while(1){}
}

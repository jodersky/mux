#include <stddef.h>
#include <sched/sched.h>
#include <time/timer.h>
#include <bug/panic.h>
#include <bug/debug.h>
#include <tshield/tshield.h>
#include <collection/list.h>
#include <serial/serial.h>

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
    sleep(500);
    debug_led(id,0);
    sleep(500);
  }
}

#define READ_BUFFER_SIZE 24
void read(char id) {
  serial_init(115200);
  char data[READ_BUFFER_SIZE];
  while(1) {
    size_t length = serial_read(data, READ_BUFFER_SIZE);
    serial_write(data, length);
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

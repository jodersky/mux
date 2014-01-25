#include "sched/sched.h"
#include "sched/idle.h"
#include "time/timer.h"
#include "mcu/sched/context.h"

struct tcb_t* volatile current = 0;
DECLARE_TASK(idle, IDLE_STACK_SIZE, idle_entry);

struct list_head ready = LIST_HEAD_INIT(ready);
static struct list_head sleeping = LIST_HEAD_INIT(sleeping);
char* volatile kstack;

static void init_idle() {
  idle.sp = init_stack(idle.mem_low, idle.mem_high, idle.entry, 0);
  INIT_LIST_HEAD(&idle.q);
}

void sched_init() {
  init_idle();
  init_kstack((char **) &kstack);
  schedule();
  sei();
  RESTORE_CONTEXT();
  RETURN();
}

void schedule() {
  if(!list_empty(&ready)) {

    if (current == &idle) {
      debug_led(0,0);
    }

    current = list_entry(ready.next, struct tcb_t, q);
    list_move_tail(ready.next, &ready);
  } else {
    current = &idle;
  }
}

void sched_tick() {
  struct tcb_t* it;
  struct tcb_t* tmp;
  list_for_each_entry_safe(it, tmp, &sleeping, q) {
    it->sleep_left -= 1000 / HZ;
    if (it->sleep_left <= 0) {
      list_move_tail(&it->q, &ready);
    }
  }
  schedule(); //in a round-robin scheduler, scheduling is called after every tick
}

void wake_all(struct list_head* queue) {
  list_splice_init(queue, ready.prev);
  if (current == &idle) {
    schedule();
  }
}

void spawn(struct tcb_t* const tcb, char args) {
  tcb->sp = init_stack(tcb->mem_low, tcb->mem_high, tcb->entry, args);
  INIT_LIST_HEAD(&tcb->q);
  list_add_tail(&tcb->q, &ready);
}

void yield(void) {
  cli();
  SAVE_CONTEXT();
  schedule();
  RESTORE_CONTEXT();
  sei();
  RETURN();
}

void sleep(long ms) {
  cli();
  SAVE_CONTEXT();
  current->sleep_left = ms;
  sleep_on(&sleeping);
  schedule();
  RESTORE_CONTEXT();
  sei();
  RETURN();
}
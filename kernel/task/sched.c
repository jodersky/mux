#include "bug/panic.h"
#include "task/task.h"
#include "task/sched.h"
#include "mcu/task/context.h"
#include "mcu/task/context.h"


struct tcb_t* volatile current = 0;

struct list_head ready = LIST_HEAD_INIT(ready);

void* volatile kstack;

void spawn(struct tcb_t* const tcb) {
  tcb->sp = stack_init(tcb->mem_low, tcb->mem_high, tcb->entry, tcb->id);
  INIT_LIST_HEAD(&tcb->queue);
  list_add_tail(&tcb->queue, &ready);
}

void yield(void) {
  cli();
  context_save();
  schedule();
  context_restore();
  sei();
  ret();
}


void sched_init() {
  kstack_init(&kstack);
  schedule();
  context_restore();
  sei();
  ret();
}

void schedule() {
  if(!list_empty(&ready)) {
    current = list_entry(ready.next, struct tcb_t, queue);
    list_move_tail(ready.next, &ready);
  } else {
    panic();
  }
}
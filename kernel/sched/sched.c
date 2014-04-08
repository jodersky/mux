#include "mux/sched.h"
#include "mcu/context.h"


struct tcb_t* volatile current = 0;

static struct tcb_t* volatile idle = 0;

struct list_head ready = LIST_HEAD_INIT(ready);

static void _spawn(struct tcb_t* const tcb, char id) {
  tcb->id = id;
  tcb->sp = stack_init(tcb->mem_low, tcb->mem_high, tcb->entry, tcb->id);
  INIT_LIST_HEAD(&tcb->queue);
}

void spawn(struct tcb_t* const tcb, char id) {
  _spawn(tcb, id);
  list_add_tail(&tcb->queue, &ready);
}

void spawn_idle(struct tcb_t* const tcb, char id) {
  _spawn(tcb, id);
  idle = tcb;
}

void yield(void) {
  context_save();
  schedule();
  context_restore();
  ret();
}

void sched_init() {
  schedule();
  context_restore();
  sei();
  ret();
}

#include <avr/io.h>
void toggle_led() {
  DDRB |= (1 << 7);
  PORTB ^= (1 << 7);
}

void schedule() {
  toggle_led();
  if(!list_empty(&ready)) {
    current = list_entry(ready.next, struct tcb_t, queue);
    list_move_tail(ready.next, &ready);
    debug_led(0,1);
  } else {
    current = idle;
    debug_led(0,0);
  }
}
#ifndef SCHED_H
#define SCHED_H

#include "collection/list.h"
#include "mcu/sched/context.h"

/** Proposed default stack size, you may use this definition for declaring tasks. */
#define STACK_SIZE 256

/** Stack size to be allocated for the idle task. */
#define IDLE_STACK_SIZE 64

/**
 * Task control block, contains runtime
 * information about tasks.
 */
struct tcb_t {
  /** Stack pointer of this task. (must be first in structure) */
  char* volatile sp;

  /** Lowest address of this task's memory (inclusive). */
  char* mem_low;

  /** Highest address of this task's memory (inclusive). */
  char* mem_high;

  /** Entry function of this task. */
  void (*entry)(char);

  /** Current wait queue that this task is in. */
  struct list_head q;

  long sleep_left;

};

/**
 * Utility for declaring a task with statically allocated memory.
 * Note: for a task to be scheduled, it must first be spawned (see spawn()).
*/
#define DECLARE_TASK(name, stack_size, entry_function) \
  static char declared_stack_##name[stack_size]; \
  static struct tcb_t name = { \
    .sp = 0, \
    .mem_low = declared_stack_##name, \
    .mem_high = declared_stack_##name + stack_size - 1, \
    .entry = entry_function, \
    .q = {}, \
    .sleep_left = 0 \
  };

/**
 * Points to currently executing task. If no scheduling has been enabled,
 * this points to NULL
 */
extern struct tcb_t* volatile current;

/**
 * Queue that contains all tasks that are ready to be run, awaiting their
 * turn from the scheduler.
 */
extern struct list_head ready;

/**
 * Stack pointer for operations performed out of task context, including any
 * calls made after SAVE_CONTEXT().
 */
extern char* volatile kstack;

/** 
 * Makes the current task sleep on a specific queue.
 * This moves the current task to the given queue's tail.
 */
static inline void sleep_on(struct list_head* queue) {
  list_move_tail(&current->q, queue);
}

/** 
 * Wakes all tasks waiting in the given queue.
 * This moves all tasks contained in the queue to the ready queue.
 */
void wake_all(struct list_head* queue);

/**
 * Initializes the scheduler by setting up kstack, initializing the idle task
 * and selecting the first task to run.
 */
void sched_init();

/** 
 * Enters the scheduler, setting current to the next runnable task.
 */
void schedule();

/**
 * Ticks the scheduler.
 */
void sched_tick();

/**
 * Initializes a given task and adds it to the ready queue.
 */
void spawn(struct tcb_t* const tcb, char args);

/**
 * Voluntarily yields control of the CPU to the scheduler.
 */
void yield() __attribute__ ( ( naked ) );

void sleep(long ms) __attribute__ ( ( naked) );

#define ENTER_CRITICAL() cli()
#define EXIT_CRITICAL() sei()

#endif

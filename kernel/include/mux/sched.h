#ifndef MUX_SCHED_H
#define MUX_SCHED_H

#include "mux/list.h"

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

  /** Current wait queue that this task is in.*/
  struct list_head queue;

  /** ID of task. */
  char id;

};

/**
 * Utility for declaring a task with statically allocated memory.
 * Note: for a task to be scheduled, it must first be spawned (see spawn()).
*/
#define DECLARE_TASK(name, stack_size, entry_function) \
  static char _declared_stack_##name[stack_size]; \
  static struct tcb_t name = { \
    .sp = 0, \
    .mem_low = _declared_stack_##name, \
    .mem_high = _declared_stack_##name + stack_size - 1, \
    .entry = entry_function, \
    .queue = {} \
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
 * Makes the current task sleep on a specific queue.
 * This moves the current task to the given queue's tail.
 */
static inline void sleep_queue(struct list_head* queue) {
  list_move_tail(&current->queue, queue);
}

/** 
 * Wakes all tasks waiting in the given queue.
 * This moves all tasks contained in the queue to the ready queue.
 */
static inline void wake_all_queue(struct list_head* queue) {
  list_splice_init(queue, ready.prev);
}

/**
 * Initializes a given task and adds it to the ready queue.
 */
void spawn(struct tcb_t* const tcb, char id);

/**
 * Spawns the idle task
 */
void spawn_idle(struct tcb_t* const tcb, char id);

/**
 * Voluntarily relinquishes control of the CPU from the current task to the scheduler.
 */
void yield() __attribute__ ( ( naked ) );


/**
 * Initializes the scheduler by setting up kstack, initializing the idle task
 * and selecting the first task to run.
 */
void sched_init();

/** 
 * Enters the scheduler, setting current to the next runnable task.
 */
void schedule();

#endif

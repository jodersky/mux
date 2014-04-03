#ifndef TASK_H
#define TASK_H

#include "collection/list.h"

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
#define DECLARE_TASK(name, stack_size, entry_function, pid) \
  static char _declared_stack_##name[stack_size]; \
  static struct tcb_t name = { \
    .sp = 0, \
    .mem_low = _declared_stack_##name, \
    .mem_high = _declared_stack_##name + stack_size - 1, \
    .entry = entry_function, \
    .queue = {}, \
    .id = pid \
  };

#endif
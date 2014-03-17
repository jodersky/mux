#ifndef SCHED_H
#define SCHED_H

#include "collection/list.h"
#include "task/task.h"
#include "mcu/task/task.h"

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
extern void* volatile kstack;

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
void spawn(struct tcb_t* const tcb);

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

#ifndef LOCK_H
#define LOCK_H

#include "task/shed.h"

typedef char spin_lock_t;

#define SPIN_LOCK_UNLOCKED 0

static inline void spin_lock(struct spin_lock_t* lock) {
  while(*lock != SPIN_LOCK_UNLOCKED) {yield();};
  cli();
  *lock = 1;
  sei();
}

static inline void spin_unlock(struct spin_lock_t* lock) {
  cli();
  *lock = SPIN_LOCK_UNLOCKED;
  sei();
}

#endif
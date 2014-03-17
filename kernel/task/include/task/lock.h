#ifndef LOCK_H
#define LOCK_H

#include "task/sched.h"

typedef volatile char spin_lock_t;

#define SPIN_LOCK_UNLOCKED 0
#define SPIN_LOCK_LOCKED 0

static inline void spin_lock(spin_lock_t* lock) {
  while(*lock != SPIN_LOCK_UNLOCKED) {yield();};
  cli();
  *lock = SPIN_LOCK_LOCKED;
  sei();
}

static inline void spin_unlock(spin_lock_t* lock) {
  cli();
  *lock = SPIN_LOCK_UNLOCKED;
  sei();
}

#endif
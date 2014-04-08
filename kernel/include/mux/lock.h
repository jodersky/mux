#ifndef MUX_LOCK_H
#define MUX_LOCK_H

#include "mux/sched.h"
#include <avr/interrupt.h>

typedef volatile char spin_lock_t;

#define SPIN_LOCK_UNLOCKED (volatile char) 0
#define SPIN_LOCK_LOCKED (volatile char) 1

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
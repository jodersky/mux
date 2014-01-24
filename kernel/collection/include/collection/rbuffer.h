#ifndef RBUFFER_H
#define RBUFFER_H

#include <stddef.h>

#define EEMPTY -1
#define EFULL -2

typedef char rbuffer_byte_t;

struct rbuffer_t {
  rbuffer_byte_t* address;
  size_t size;
  size_t head;
  size_t tail;
};

#define RBUFFER_INIT(array, length) \
  { \
    .address = array, \
    .size = length, \
    .head=0, \
    .tail=0 \
  }

void rbuffer_init(struct rbuffer_t* rb, rbuffer_byte_t* address, size_t size);

/** Read data from ringbuffer. */
static inline int rbuffer_read(struct rbuffer_t* const rb, rbuffer_byte_t* const data_out) {
  if (rb->head == rb->tail) return EEMPTY;
  *data_out = rb->address[rb->head];
  rb->head = rb->head + 1;
  if (rb->head >= rb->size) rb->head=0;
  return 0;
}

static inline int rbuffer_write(struct rbuffer_t* const rb, rbuffer_byte_t data_in) {
  size_t next_tail = rb->tail + 1;
  if (next_tail >= rb->size) next_tail = 0;  
  if (rb->head == next_tail) return EFULL;
  
  rb->address[rb->tail] = data_in;
  rb->tail = next_tail;
  return 0;
}

#endif

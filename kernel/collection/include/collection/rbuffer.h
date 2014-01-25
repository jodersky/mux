#ifndef RBUFFER_H
#define RBUFFER_H

#include <stddef.h>

struct rbuffer_t {
  char* address_begin;
  char* address_end;
  char* volatile head;
  char* volatile tail;
};

#define RBUFFER_ARRAY_INIT(array, length) \
  { \
    .address_begin = array, \
    .address_end = array + length, \
    .head=array, \
    .tail=array \
  }

static inline int rbuffer_empty(struct rbuffer_t* const rb) {
  return rb->head == rb->tail;
}

#define __rbuffer_overlap(rb, ptr) \
  if (ptr >= rb->address_end) ptr = rb->address_begin

static inline int rbuffer_write_char(struct rbuffer_t* const rb, char data) {
  char* next_head = rb->head + 1;
  __rbuffer_overlap(rb, next_head);
  if (rb->tail == next_head) return 0;
  *rb->head = data;
  rb->head = next_head;
  return 1;
}

static inline int rbuffer_read_char(struct rbuffer_t* const rb, char* const data) {
  if (rbuffer_empty(rb)) return 0;
  *data = *rb->tail;
  rb->tail += 1;
  __rbuffer_overlap(rb, rb->tail);
  return 1;
}

static inline size_t rbuffer_write(struct rbuffer_t* const rb, const char* const data, size_t size) {
  size_t wrote = 0;
  for (size_t i = 0; i < size; ++i) {
    if (rbuffer_write_char(rb, data[i])) {
      wrote += 1;
    } else {
      return wrote;
    }
  }
  return wrote;
}

static inline size_t rbuffer_read(struct rbuffer_t* const rb, char* const data, size_t size) {
  size_t read = 0;
  for (size_t i = 0; i < size; ++i) {
    if (rbuffer_read_char(rb, &data[i])) {
      read += 1;
    } else {
      return read;
    }
  }
  return read;
}  


#endif

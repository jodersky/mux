#ifndef RBUFFER_H
#define RBUFFER_H

#include <stddef.h>

struct rbuffer_t {
    char* address_begin;
    char* address_end;
    char* volatile head;
    char* volatile tail;
};

#define RBUFFER_INIT(array, length) \
    { \
        .address_begin = array, \
        .address_end = array + length, \
        .head = array, \
        .tail = array \
    }

#define INIT_RBUFFER(buffer, array, length) \
    (buffer)->address_begin = (array); \
    (buffer)->address_end = (array) + (length); \
    (buffer)->head = (array); \
    (buffer)->tail = (array)

static inline int rbuffer_empty(struct rbuffer_t* const rb) {
    return rb->head == rb->tail;
}

static inline int rbuffer_write(struct rbuffer_t* const rb, char data) {
    char* next_head = rb->head + 1;
    if(next_head >= rb->address_end) next_head = rb->address_begin;
    if (rb->tail == next_head) return -1;
    *rb->head = data;
    rb->head = next_head;
    return 0;
}

static inline int rbuffer_read(struct rbuffer_t* const rb, char* const data) {
    if (rbuffer_empty(rb)) return -1;
    *data = *rb->tail;
    rb->tail += 1;
    if(rb->tail >= rb->address_end) rb->tail = rb->address_begin;
    return 0;
}

#endif

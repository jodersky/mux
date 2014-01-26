#ifndef SERIAL_H
#define SERIAL_H

#include <stddef.h>
#include <stdio.h>

#include "collection/rbuffer.h"
#include "collection/list.h"

#define USARTS 1
#define SERIAL_BUFFER_SIZE 64

struct serial_device_t {
  volatile char __rx_buffer[SERIAL_BUFFER_SIZE];
  volatile char __tx_buffer[SERIAL_BUFFER_SIZE];

  struct rbuffer_t rx_buffer;
  struct rbuffer_t tx_buffer;

  struct list_head rx_q;
  struct list_head tx_q;
};

#define SERIAL_DEVICE_INIT(name) \
  { \
    .rx_buffer = RBUFFER_ARRAY_INIT(name.__rx_buffer, SERIAL_BUFFER_SIZE), \
    .tx_buffer = RBUFFER_ARRAY_INIT(name.__tx_buffer, SERIAL_BUFFER_SIZE), \
    .rx_q = LIST_HEAD_INIT(name.rx_q), \
    .tx_q = LIST_HEAD_INIT(name.tx_q) \
  }

void serial_init(unsigned long baud);

size_t serial_read(char* const data, size_t size);

size_t serial_write(const char* const data, size_t size);

inline void serial_write_str(const char* const str) {
  size_t length = 0;
  for (length = 0; str[length] != 0; ++length){}
  serial_write(str, length);
}

int serial_getc();

int serial_putc(char c);

extern FILE serial_in;
extern FILE serial_out;

#endif
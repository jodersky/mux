#ifndef SERIAL_H
#define SERIAL_H

#include "collection/rbuffer.h"
#include "collection/list.h"

#define USARTS 1
#define USART_BUFFER_SIZE 64

struct usart_device_t {
  volatile char __rx_buffer[USART_BUFFER_SIZE];
  volatile char __tx_buffer[USART_BUFFER_SIZE];

  struct rbuffer_t rx_buffer;
  struct rbuffer_t tx_buffer;

  struct list_head rx_queue;
  struct list_head tx_queue;
};

#define USART_DEVICE_INIT(name) \
  { \
    .rx_buffer = RBUFFER_ARRAY_INIT(name.__rx_buffer, USART_BUFFER_SIZE), \
    .tx_buffer = RBUFFER_ARRAY_INIT(name.__tx_buffer, USART_BUFFER_SIZE), \
    .rx_queue = LIST_HEAD_INIT(name.rx_queue), \
    .tx_queue = LIST_HEAD_INIT(name.tx_queue) \
  }

void usart_init(unsigned long baud);

#endif
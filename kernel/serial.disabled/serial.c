#include <avr/interrupt.h>
#include "collection/list.h"
#include "sched/sched.h"
#include "sched/context.h"
#include "bug/debug.h"
#include "serial/serial.h"

static struct serial_device_t serial = SERIAL_DEVICE_INIT(serial);

void serial_init(unsigned long baud) {
  UCSR0A |= (1 << U2X0);   //enable double speed transmission
  uint16_t baud_setting = (F_CPU / 4 / baud - 1) / 2;
  UBRR0H = baud_setting >> 8;
  UBRR0L = baud_setting;
  UCSR0B |= (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0); // 8-bit, no parity, 1 stop bit
  UCSR0B &= ~(1 << UDRIE0);

  //EICRA = (EICRA & ~((1 << ISC00) | (1 << ISC01))) | (1 << ISC00);
  //EIMSK |= (1 << INT0);
}

static inline int rbuffer_safe_read(struct rbuffer_t* const rb, char* d) {
  ENTER_CRITICAL();
  int ret = rbuffer_read(rb, d);
  EXIT_CRITICAL();
  return ret;
}

size_t serial_read(char* const data, size_t max_size) {

  if (rbuffer_safe_read(&serial.rx_buffer, data) == 0) {
    size_t i;
    char d;
    for (i = 1; i < max_size && (rbuffer_safe_read(&serial.rx_buffer, &d) == 0); ++i) {
      data[i] = d;
    }
    return i;
  } else {
    ENTER_CRITICAL();
    list_move_tail(&current->q, &serial.rx_q);
    EXIT_CRITICAL();
    yield();
    return serial_read(data, max_size);
  }
}


//called when byte is received
ISR(USART0_RX_vect, ISR_NAKED) {
  SAVE_CONTEXT();
  char c = UDR0;
  rbuffer_write(&serial.rx_buffer, c);
  if (!list_empty(&serial.rx_q)) {
    list_move_tail(serial.rx_q.next, &ready);
  }
  schedule();
  RESTORE_CONTEXT();
  asm volatile ("reti");
}


void serial_write(const char* const data, size_t size) {
  for (size_t i = 0; i < size; ++i) {
    ENTER_CRITICAL();
    rbuffer_write(&serial.tx_buffer, data[i]);
    EXIT_CRITICAL();
    UCSR0B |= (1 << UDRIE0);
  }
}

//called when data register is empty
ISR(USART0_UDRE_vect) {
  char c;
  if (rbuffer_read(&serial.tx_buffer, &c) == 0) {
    UDR0 = c;
  } else {
    UCSR0B &= ~(1 << UDRIE0); //buffer empty, disable interruot
  }
}
/*
void serial_read() {
  ENTER_CRITICAL();
  list_move_tail(&current->list, &serial_rx_q);
  EXIT_CRITICAL();
  yield();
}
*/
/*
void serial_write_str(const char* const str) {
  for (size_t i = 0; str[i] != '\0'; ++i) {
    ENTER_CRITICAL();
    rbuffer_write(&serial_dev.tx_buffer, str[i]);
    EXIT_CRITICAL();
    UCSR0B |= (1 << UDRIE0);
  }
}*/


/*
ISR(INT3_vect, ISR_NAKED) {
  SAVE_CONTEXT();
  if (!list_empty(&serial_rx_q)) {
    list_move_tail(serial_rx_q.next, &ready);
  }
  schedule();
  RESTORE_CONTEXT();
  asm volatile ("reti");
}
*/

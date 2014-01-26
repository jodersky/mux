#include <avr/interrupt.h>
#include "serial/serial.h"
#include "sched/sched.h"
#include "bug/debug.h"

 FILE serial_out = FDEV_SETUP_STREAM(serial_putc, NULL, _FDEV_SETUP_WRITE);
 FILE serial_in = FDEV_SETUP_STREAM(NULL, serial_getc, _FDEV_SETUP_READ);

static struct serial_device_t serial = SERIAL_DEVICE_INIT(serial);

void serial_init(unsigned long baud) {
  UCSR0A |= (1 << U2X0);   //enable double speed transmission
  uint16_t baud_setting = (F_CPU / 4 / baud - 1) / 2;
  UBRR0H = baud_setting >> 8;
  UBRR0L = baud_setting;
  UCSR0B |= (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0); // 8-bit, no parity, 1 stop bit
  UCSR0B &= ~(1 << UDRIE0);
}

size_t serial_read(char* const data, size_t size) {
 while (rbuffer_empty(&serial.rx_buffer)) {
    cli();
    sleep_on(&serial.rx_q);
    yield();
  }
  cli();
  size_t r = rbuffer_read(&serial.rx_buffer, data, size);
  sei();
  return r;
}

size_t serial_write(const char* const data, size_t size) {
  cli();
  size_t r = rbuffer_write(&serial.tx_buffer, data, size);
  sei();
  UCSR0B |= (1 << UDRIE0);
  return r;
}

int serial_getc() {
  while (rbuffer_empty(&serial.rx_buffer)) {
    cli();
    sleep_on(&serial.rx_q);
    yield();
  }
  cli();
  char c = 0;
  size_t r = rbuffer_read_char(&serial.rx_buffer, &c);
  sei();
  if (r) return (int) c;
  else return EOF;
}

int serial_putc(char c) {
  if (c == '\n') serial_putc('\r');
  int r = 0;
  do {
    cli();
    r = rbuffer_write_char(&serial.tx_buffer, c);
    sei();
    UCSR0B |= (1 << UDRIE0);
  } while (r != 1);
  return c;
}

//called when byte is received
ISR(USART0_RX_vect, ISR_NAKED) {
  SAVE_CONTEXT();
  char c = UDR0;
  rbuffer_write_char(&serial.rx_buffer, c);
  wake_all(&serial.rx_q);
  RESTORE_CONTEXT();
  asm volatile ("reti");
}

//called when data register is empty
ISR(USART0_UDRE_vect) {
  char c;
  if (rbuffer_read_char(&serial.tx_buffer, &c)) {
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

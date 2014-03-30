
#include <stddef.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include "task/sched.h"
#include "io/usart.h"
#include "bug/debug.h"
#include "mcu/task/context.h"

static struct usart_device_t umain = USART_DEVICE_INIT(umain);
static int umain_putc(char c);
static int umain_getc();

FILE umain_out = FDEV_SETUP_STREAM(umain_putc, NULL, _FDEV_SETUP_WRITE);
FILE umain_in = FDEV_SETUP_STREAM(NULL, umain_getc, _FDEV_SETUP_READ);

void usart_init(unsigned long baud) {
  UCSR0A |= (1 << U2X0);   //enable double speed transmission
  uint16_t baud_setting = (F_CPU / 4 / baud - 1) / 2;
  UBRR0H = baud_setting >> 8;
  UBRR0L = baud_setting;
  UCSR0B |= (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0); // 8-bit, no parity, 1 stop bit
  UCSR0B &= ~(1 << UDRIE0);
  stdout = &umain_out;
  stdin = &umain_in;
}

int umain_putc(char c) {
  if (c == '\n') umain_putc('\r');
  int r = 0;
  do {
    cli();
    r = rbuffer_write_char(&umain.tx_buffer, c);
    sei();
    UCSR0B |= (1 << UDRIE0);
  } while (r != 1);
  return c;
}

int umain_getc() {
  while (rbuffer_empty(&umain.rx_buffer)) {
    cli();
    sleep_queue(&umain.rx_queue);
    yield();
  }
  cli();
  char c = 0;
  size_t r = rbuffer_read_char(&umain.rx_buffer, &c);
  sei();
  if (r) return (int) c;
  else return EOF;
}

//called when byte is received
ISR(USART0_RX_vect, ISR_NAKED) {
  context_save();
  char c = UDR0;
  rbuffer_write_char(&umain.rx_buffer, c);
  wake_all_queue(&umain.rx_queue);
  context_restore();
  asm volatile ("reti");
}

//called when data register is empty
ISR(USART0_UDRE_vect) {
  char c;
  if (rbuffer_read_char(&umain.tx_buffer, &c)) {
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

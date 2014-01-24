#include <avr/io.h>
#include "tshield/tshield.h"

static void tshield_init_servo();

void tshield_init() {
  
  DDRH |= (1 << 3) | (1 << 4); //leds
  PORTH &= ~( (1 << 3) | (1 << 4));
  DDRE |= (1 << 3);
  PORTE &= ~(1 << 3);
  DDRG |= (1 << 5);
  PORTG &= ~(1 << 5);
  
  DDRE &= ~(1 << 4); // buttons
  PORTE |= (1 << 4);
  DDRH &= ~(1 << 5);
  PORTH |= (1 << 5);
  DDRB &= ~(1 << 4);
  PORTB |= (1 << 4);
  
  DDRB |= (1 << 6);
  PORTB &= ~(1 << 6);

  tshield_init_servo();
}

#define WAIT_VALUE 20000
#define WAIT() \
  for (volatile long x = 0; x < WAIT_VALUE; ++x){}
  
void tshield_test() {
  while(1) {
    PORTG |= (1 << 5);  
    WAIT();
    PORTG &= ~(1 << 5);
    PORTE |= (1 << 3);
    WAIT();
    PORTE &= ~(1 << 3);
    PORTH |=  (1 << 3);
    WAIT();
    PORTH &=  ~(1 << 3);
    PORTH |=  (1 << 4);
    WAIT();
    PORTH &=  ~(1 << 4);
  }
  
}

unsigned char tshield_read() {
  return ((PINE & (1 << 4) ? 0 : 1) << 2 ) |
  ((PINH & (1 << 5) ? 0 : 1) << 1 ) |
  (PINB & (1 << 4) ? 0 : 1);
}

void tshield_led(unsigned char led, unsigned char value) {
  volatile unsigned char* port;
  unsigned char bit;
  switch (led) {
    case 0:
      port = &PORTG;
      bit = 5;
      break;
    case 1:
      port = &PORTE;
      bit = 3;
      break;
    case 2:
      port = &PORTH;
      bit = 3;
      break;
    case 3:
      port = &PORTH;
      bit = 4;
      break;
    default:
      return;
  }

  if (value) {
    *port |= (1 << bit);
  } else {
    *port &= ~(1 << bit);
  }

}

void tshield_pp(unsigned char value) {
  if (value) {
    PORTB |= (1 << 6);
  } else {
    PORTB &= ~(1 << 6);
  }
}

void tshield_init_servo() {
  
  TCCR1A |= (1<<COM1A1); // non inverted pwm on channel A
  TCCR1B |= (1<<CS11) | (1<<CS10); // prescaler 64

  //fast pwm
  TCCR1A |= (1<<WGM11);
  TCCR1B|=(1<<WGM13) | (1<<WGM12);

  ICR1=4999; //fPWM=50Hz (Period = 20ms Standard).

  DDRB |= (1<< 5);
}

void tshield_servo(unsigned char angle) {
  unsigned long k = (unsigned long) angle;
  unsigned long counter = (450 * k) / 255 + 150;
  OCR1A = (unsigned int) counter;
}

void debug_led(int led, int value) {
  tshield_led(led, value);
}

static inline void _wait() {
  for (volatile long i = 0; i < 20000; ++i) {};
}

void panic() {
  cli();
  while(1) {
    for(int i = 0; i < DEBUG_LEDS; ++i) {
      debug_led((i - 1) % DEBUG_LEDS, 0);
      debug_led(i, 1);
      _wait();
    }
    for(int i = DEBUG_LEDS - 1; i >= 0; --i) {
      debug_led((i + 1) % DEBUG_LEDS, 0);
      debug_led(i, 1);
      _wait();
    }
  }
}
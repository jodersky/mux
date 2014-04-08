#ifndef TSHIElD_H
#define TSHIELD_H

/*
 * Custom test shield
 * Pin mappings
 * 
 * Leds (from left to right)
 * =====
 * Color   | Arduino pin | atmega2560
 * ----------------------------------
 * Green     D6            PH3
 * Yellow    D7            PH4
 * Red       D5            PE3
 * Red       D4            PG5
 * 
 * 
 * Buttons (from left to right)
 * =====
 * Number | Arduino pin | atmega2560
 * ---------------------------------
 * 0        D2            PE4 (INT4)
 * 1        D8            PH5
 * 2        D10           PB4
 * 
 * Outputs (from top to bottom)
 * =====
 * Type                        | Arduino pin | atmega2560
 * ------------------------------------------------------
 * Servo                         11, Vcc, GND  PB5
 * Diode protected (max 200mA)   12            PB6
 * 
 */

#define DEBUG_LEDS 4
#define DEBUG_LED_IDLE 0
 
void tshield_init();

void tshield_test();

unsigned char tshield_read();

void tshield_led(unsigned char led, unsigned char value);

void tshield_pp(unsigned char value);

void tshield_servo(unsigned char angle);


#endif

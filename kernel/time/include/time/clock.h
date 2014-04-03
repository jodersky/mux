#ifndef CLOCK_H
#define CLOCK_H

/**
 * Initializes main system clock.
 * @ms time between each tick in milliseconds
 * @tick function to run every tick
 */
void clock_init(int ms, void (*tick)());

/**
 * Starts main system clock.
 */
void clock_start();

/**
 * Stops main system clock.
 */
void clock_stop();

#endif
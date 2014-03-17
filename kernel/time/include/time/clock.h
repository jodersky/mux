#ifndef CLOCK_H
#define CLOCK_H

void clock_init(int ms, void (*tick)());
void clock_start();
void clock_stop();

#endif
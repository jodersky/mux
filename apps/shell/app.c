#include "init.h"
#include "tshield/tshield.h"
#include "mux/sched.h"
#include "mux/io.h"
#include "mcu/usart.h"
#include "mcu/context.h"
#include <stdio.h>

#define BUFFER_SIZE 64
#define WAIT_CYCLES(cycles) for (unsigned volatile long i = 0; i < cycles; ++i){}

void setup() {
    tshield_init();
}

void task1(char args) {
    open(&usart0);
    ioctl(&usart0, IOCTL_SET_BAUD, 115200);

    char in[BUFFER_SIZE];

    while(1) {
        int length = read(&usart0, in, BUFFER_SIZE);

        int led;
        int value;
        if (sscanf(in, "leds/%d:%d", &led, &value) == 2) {
            debug_led(led, value);
        } else if (sscanf(in, "servo/0:%d", &value) == 1) {
            tshield_servo((char) value);
        } else {
            char* msg = "unknown command";
            write(&usart0, msg, 15);
            debug_led(0,1);
            WAIT_CYCLES(300000);
            debug_led(0,0);
        }
    }
}

void task2(char args) {
    while(1){

    }
}

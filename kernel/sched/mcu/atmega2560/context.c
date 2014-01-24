#include "mcu/sched/context.h"

char* init_stack(const char* const mem_low, const char* const mem_high, void (*entry)(char), char args) {
  char* sp = (char*) mem_high;
  unsigned long address = (unsigned long) entry;
  
  *sp = (char) 0x1;
  sp--;
  *sp = (char) 0x2;
  sp--;
  *sp = (char) 0x1;
  sp--;
  *sp = (char) 0x2;
  sp--;
  
	*sp = (char) ( address & (unsigned short) 0x00ff );
	sp--;

	*sp = (char) ( (address >> 8) & ( unsigned short ) 0x00ff );
	sp--;
  
  *sp = (char) ( (address >> 16) & ( unsigned short ) 0x00ff );
	sp--;

	*sp = (char) 0x00; //r0
	sp--;
	*sp = (char) 0x80; //SREG, enable interrupts when task starts
	sp--;
	*sp = ( char ) 0x00; //r1
	sp--;
	*sp = ( char ) 0x00; //r2
	sp--;
	*sp = ( char ) 0x00; //r3
	sp--;
	*sp = ( char ) 0x00; //r4
	sp--;
	*sp = ( char ) 0x00; //r5
	sp--;
	*sp = ( char ) 0x00; //r6
	sp--;
	*sp = ( char ) 0x00; //r7
	sp--;
	*sp = ( char ) 0x00; //r8
	sp--;
	*sp = ( char ) 0x00; //r9
	sp--;
	*sp = ( char ) 0x00; //r10
	sp--;
	*sp = ( char ) 0x00; //r11
	sp--;
	*sp = ( char ) 0x00; //r12
	sp--;
	*sp = ( char ) 0x00; //r13
	sp--;
	*sp = ( char ) 0x00; //r14
	sp--;
	*sp = ( char ) 0x00; //r15
	sp--;
	*sp = ( char ) 0x00; //r16
	sp--;
	*sp = ( char ) 0x00; //r17
	sp--;
	*sp = ( char ) 0x00; //r18
	sp--;
	*sp = ( char ) 0x00; //r19
	sp--;
	*sp = ( char ) 0x00; //r20
	sp--;
  *sp = ( char ) 0x00; //r21
	sp--;
  *sp = ( char ) 0x00; //r22
	sp--;
  *sp = ( char ) 0x00; //r23
	sp--;
  
  *sp = (char) ( args ); //place first argument in register 24
	sp--;
  
  *sp = ( char ) 0x00; //r25
	sp--;
  *sp = ( char ) 0x00; //r26
	sp--;
  *sp = ( char ) 0x00; //r27
	sp--;
  *sp = ( char ) 0x00; //r28
	sp--;
  *sp = ( char ) 0x00; //r29
	sp--;
  *sp = ( char ) 0x00; //r30
	sp--;
  *sp = ( char ) 0x00; //r31
	sp--;
  
  return sp;
}

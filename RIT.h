#ifndef RIT_H_
#define RIT_H_

#include <LPC17xx.h>
extern uint8_t timer_done;

void RIT_IRQHandler();
void rit_init(void);
 
#endif
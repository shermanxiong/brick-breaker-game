#include <RTL.h>
#include "RIT.h"
#include "ADC.h"

uint8_t timer_done = 0;

void rit_init(void) {
  /* Initialize hardware timer as system tick timer. */

  LPC_SC->PCONP     |=  (1<<16);   // Enable power for RIT
  LPC_SC->PCLKSEL1  &= ~(3<<26);
  LPC_SC->PCLKSEL1  |=  (1<<26);   // RIT Clock = CCLK

  LPC_RIT->RICOMPVAL = 100000;     // Set match value
  LPC_RIT->RICOUNTER = 0;          // Set count value to 0
  LPC_RIT->RICTRL    = (1<<3) |    // Timer enable
                       (1<<1) |    // Enable clear on match
                       (1<<0);     // Clear interrupt flag
	//enable interrupt
	NVIC_SetPriority(RIT_IRQn, 31);
	NVIC_EnableIRQ(RIT_IRQn);
}
 
void RIT_IRQHandler()
{
	timer_done = 1;
	
}

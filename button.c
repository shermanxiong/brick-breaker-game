#include "LPC17xx.H"                         /* LPC17xx definitions           */
#include "button.h"

uint8_t button_done = 0;

void button_init(void){
	LPC_PINCON->PINSEL4  &= ~( 3 << 20 );
	LPC_GPIO2->FIODIR    &= ~( 1 << 10 );
	LPC_GPIOINT->IO2IntEnF   |= ( 1 << 10 );
	NVIC_EnableIRQ( EINT3_IRQn );
}

void EINT3_IRQHandler(void) {
	LPC_GPIOINT->IO2IntClr |= ( 1 << 10 );
	button_done = 1;
}

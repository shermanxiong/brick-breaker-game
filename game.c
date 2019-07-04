#include <stdio.h>
#include <RTL.h>
#include <LPC17xx.h>                         // LPC17xx definitions           */
#include "GLCD.h"
#include "LED.h"
#include "game.h"
//#include "RIT.h"
#include "ADC.h"
#include "button.h"

#define X_MIN 0
#define X_MAX 320
#define Y_MIN 0
#define Y_MAX 240
#define W 0xFFFF
#define B 0x0000

OS_MUT mut_GLCD;
OS_SEM sem_ADC;

paddle_t paddle;
ball_t ball;
brick_state_t brick_state;
brick_t bricks [BRICKS_X][BRICKS_Y];

uint8_t GAME_OVER;
uint16_t LIVES;
uint32_t BRICK_NUM;
uint8_t WIN; // 0 = LOSE, 1 = WIN

uint32_t ad_avg;
uint16_t ad_val;
uint16_t ad_val_;

unsigned short bricks_bitmap[BRICKS_WIDTH * BRICKS_HEIGHT] = { B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B,
																															B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B,
																															B, B, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, B, B,
																															B, B, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, B, B,
																															B, B, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, B, B,
																															B, B, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, B, B,
																															B, B, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, B, B,
																															B, B, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, B, B,
																															B, B, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, B, B,
																															B, B, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, B, B,
																															B, B, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, B, B,
																															B, B, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, B, B,
																															B, B, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, B, B,
																															B, B, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, B, B,
																															B, B, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, B, B,
																															B, B, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, B, B,
																															B, B, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, B, B,
																															B, B, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, B, B,
																															B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B,
																															B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B, B };

unsigned short bricks_bitmap_mask[BRICKS_WIDTH * BRICKS_HEIGHT] = { W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W,
																																	 W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W,
																																	 W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W,
																																	 W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W,
																																	 W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W,
																																	 W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W,
																																	 W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W,
																																	 W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W,
																																	 W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W,
																																	 W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W,
																																	 W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W,
																																	 W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W,
																																	 W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W,
																																	 W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W,
																																	 W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W,
																																	 W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W,
																																	 W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W,
																																	 W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W,
																																	 W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W,
																																	 W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W};

unsigned short ball_bitmap[BALL_DIAMETER*BALL_DIAMETER] = { 		W, W, W, W, B, B, B, W, W, W, W,
																																W, W, B, B, B, B, B, B, B, W, W,
																																W, B, B, B, B, B, B, B, B, B, W,
																																W, B, B, B, B, B, B, B, B, B, W,
																																B, B, B, B, B, B, B, B, B, B, B,
																																B, B, B, B, B, B, B, B, B, B, B,
																																B, B, B, B, B, B, B, B, B, B, B,
																																W, B, B, B, B, B, B, B, B, B, W,
																																W, B, B, B, B, B, B, B, B, B, W,
																																W, W, B, B, B, B, B, B, B, W, W,
																																W, W, W, W, B, B, B, W, W, W,	W }; 

unsigned short ball_bitmap_mask[BALL_DIAMETER*BALL_DIAMETER] = { 	W, W, W, W, W, W, W, W, W, W, W,
																																	W, W, W, W, W, W, W, W, W, W, W,
																																	W, W, W, W, W, W, W, W, W, W, W,
																																	W, W, W, W, W, W, W, W, W, W, W,
																																	W, W, W, W, W, W, W, W, W, W, W,
																																	W, W, W, W, W, W, W, W, W, W, W,
																																	W, W, W, W, W, W, W, W, W, W, W,
																																	W, W, W, W, W, W, W, W, W, W, W,
																																	W, W, W, W, W, W, W, W, W, W, W,
																																	W, W, W, W, W, W, W, W, W, W, W,
																																	W, W, W, W, W, W, W, W, W, W, W }; 
void paddle_init (){
	uint32_t i = 0;
	//initialize position to centre
	paddle.width = PADDLE_X_MAX;
	paddle.height = PADDLE_Y_MAX;
	paddle.x_pos = ((X_MIN + X_MAX)-paddle.width)/2;
	paddle.x_prev = paddle.x_pos;
	os_mut_init(paddle.mut);
	//set bitmap
	for ( i = 0; i< paddle.width*paddle.height; i++) {
		paddle.bitmap[i] = B;
		paddle.bitmap_mask[i] = W;
	}
}

void ball_init (){
	uint32_t i = 0;
	//uint32_t x = 0;
	//uint32_t y = 0;
	ball.x_vel = 0;
	ball.y_vel = 0;
	ball.diameter = BALL_DIAMETER;
	ball.x_pos = ((X_MIN + X_MAX)-ball.diameter)/2;
	ball.y_pos = Y_MAX-paddle.height-ball.diameter;
	ball.x_prev = ball.x_pos;
	ball.y_prev = ball.y_pos;
	
	//set bitmap
	
	//ball->bitmap = ball_bitmap;
	//ball->bitmap_mask = ball_bitmap_mask;
	/*
	for ( i = 0; i < ball.diameter*ball.diameter; i++){
		x = i%ball.diameter + ball.diameter/2;
		y = i/ball.diameter + ball.diameter/2;
		ball.bitmap_mask[i] = White; 
		ball.bitmap[i] = White;
		if (x*x+y*y <= (ball.diameter*ball.diameter/4)){ //draw a circle 
			ball.bitmap[i] = Black;
		}
	}*/
	
// 	for ( i = 0; i < ball.diameter*ball.diameter; i++){
// 		ball.bitmap_mask[i] = W;
// 		if((i <= 3) || (i>=7 && i<=12) || (i>=20 && i<=22) || (i == 32) || (i == 33) || (i == 43) || (i == 77) || (i== 87)|| (i == 88) || (i>=98 && i <=100) || (i>=108 && i<=113) || (i>=117)){
// 			ball.bitmap[i] = W;
// 		}else {
// 			ball.bitmap[i] = B;
// 		}
// 		
// 	}
}
void bricks_init(){
	uint32_t i = 0;
	uint32_t j = 0;
	uint32_t k = 0;
	uint32_t l = 0;
	uint32_t bitmap_val = 0;
	uint32_t bitmap_x = 0;
	uint32_t bitmap_y = 0;
	
	//populate brick state with bricks
	for(i = 0; i < BRICKS_X; i++){
		for(j = 0; j < BRICKS_Y; j++){
			bricks[i][j].x_pos = BRICKS_X_MIN+(BRICKS_WIDTH*i);
			bricks[i][j].y_pos = BRICKS_Y_MIN+(BRICKS_HEIGHT*j);			
			bricks[i][j].life = 1; // later have different life distributions for different lives?
			if(bricks[i][j].life >= 1){
				GLCD_Bitmap(bricks[i][j].x_pos, bricks[i][j].y_pos, BRICKS_WIDTH, BRICKS_HEIGHT, (unsigned char*)bricks_bitmap);
			}
		}
	}
}

__task void start_ADC(void * ptr){ //runs until game ends
		os_sem_wait(&sem_ADC,0xffff);
		{
			ADC_StartCnv();
		}os_sem_send(&sem_ADC);
	os_tsk_delete_self();
}
__task void update_graphics_task(void  * ptr){
	// loop forever
		//don't loop? only call when ball or paddle task is done 
		uint32_t i = 0;
		uint32_t j = 0;
		os_mut_wait(&mut_GLCD, 0xffff);
		{
			//paddle 

			GLCD_Bitmap(paddle.x_prev, Y_MAX-paddle.height, paddle.width, paddle.height, (unsigned char*)paddle.bitmap_mask);
			GLCD_Bitmap(paddle.x_pos, Y_MAX-paddle.height, paddle.width, paddle.height, (unsigned char*)paddle.bitmap);
			
			//ball
			GLCD_Bitmap(ball.x_prev, ball.y_prev, ball.diameter, ball.diameter, (unsigned char*)paddle.bitmap_mask);
			GLCD_Bitmap(ball.x_pos, ball.y_pos, ball.diameter, ball.diameter, (unsigned char*)paddle.bitmap);
				
		} os_mut_release(&mut_GLCD);
	os_tsk_delete_self();
}

__task void update_paddle_task(void * ptr){ //runs until game ends
//	while (1) {
		/* AD converter input                                                     */
    if (AD_done) {                           /* If conversion has finished    */
      AD_done = 0;

      ad_avg += AD_last << 8;                /* Add AD value to averaging     */
      ad_avg ++;
      if ((ad_avg & 0xFF) == 0x10) {         /* average over 16 values        */
        ad_val = (ad_avg >> 8) >> 4;         /* average devided by 16         */
        ad_avg = 0;
      }
    }

    if (ad_val ^ ad_val_) {                  /* AD value changed              */
      ad_val_ = ad_val;
			os_mut_wait(&paddle.mut,0xffff);
			paddle.x_prev = paddle.x_pos;
			paddle.x_pos = ((X_MAX-paddle.width)*ad_val)/0x0FFF;
			os_mut_release(&paddle.mut);
			os_tsk_create_ex(update_graphics_task, 2, 0);
		}
//	} 
	os_tsk_delete_self();
}

void wait_for_button(){
	while(button_done == 0){
		os_tsk_create_ex(start_ADC,2,0);
	  os_tsk_create_ex(update_paddle_task, 2, 0);
		os_mut_wait(&ball.mut,0xffff);
		ball.x_prev = ball.x_pos;
		ball.x_pos = paddle.x_pos + (paddle.width-ball.diameter)/2 ;
		os_mut_release(&ball.mut);
		os_tsk_create_ex(update_graphics_task, 2, 0);
	}
	
	button_done = 0;	
	ball.x_prev = ball.x_pos;
	ball.x_pos = paddle.x_pos + (paddle.width-ball.diameter)/2 ;
	ball.y_pos = Y_MAX-paddle.height-ball.diameter;
	ball.x_vel = 1;
	ball.y_vel = -1;
}

__task void update_ball_task(void * ptr){ //also updates brick state, runs until game ends
	uint32_t i = 0;
	uint32_t j = 0;

		// Collision detection: ball with walls
		if ((ball.x_pos + ball.diameter >= X_MAX && ball.x_vel > 0)|| (ball.x_pos <= X_MIN && ball.x_vel < 0) ){ 
			//if the ball is at the right boundary and has positive x-velocity
			//OR the ball is at the left boundary and has negative x-velocity
			os_mut_wait(&ball.mut,0xffff);
			ball.x_vel = -1*ball.x_vel;
			os_mut_release(&ball.mut);
			//replace with flip x_vel task?
		}  
		
		if (ball.y_pos <= Y_MIN && ball.y_vel < 0){
			///if the ball is at the top boundary and has negative y-velocity
			os_mut_wait(&ball.mut,0xffff);
			ball.y_vel = -1*ball.y_vel;
			os_mut_release(&ball.mut);
		}
		
		// Collision detection: ball with paddle
		if(	(ball.x_pos <= (paddle.x_pos + paddle.width) && 
				(ball.x_pos + ball.diameter) >= paddle.x_pos) &&
				(ball.y_pos + ball.diameter) >= (Y_MAX - paddle.height) &&
				(ball.y_pos <= Y_MAX)){
					
				//flip x_velocity if the collision is with one of the sides
				if(((ball.x_pos == (paddle.width + paddle.x_pos)) && (ball.x_vel < 0)) || ((ball.x_pos + ball.diameter == paddle.x_pos) && (ball.x_vel > 0))){ //do we need to account for it hitting a corner? 
					os_mut_wait(&ball.mut,0xffff);		
					ball.x_vel = -1*ball.x_vel;
					os_mut_release(&ball.mut);
				
				} else if ( ((ball.y_pos + ball.diameter) == (Y_MAX - paddle.height)) && (ball.y_vel > 0) ){
				//flip y-velocity if the collision is with the top of the paddle
					os_mut_wait(&ball.mut,0xffff);
					ball.y_vel = -1*ball.y_vel;
					os_mut_release(&ball.mut);
			  }
			
		}
		// collision detection with bricks
		for (i = 0; i< BRICKS_X; i++){
			for (j = 0; j<BRICKS_Y; j++){
				
				if(	(ball.x_pos <= (bricks[i][j].x_pos + BRICKS_WIDTH) && 
				(ball.x_pos + ball.diameter) >= bricks[i][j].x_pos) &&
				(ball.y_pos + ball.diameter) >= bricks[i][j].y_pos &&
				(ball.y_pos <= bricks[i][j].y_pos + BRICKS_HEIGHT) && 
				bricks[i][j].life != 0																		){
					
					//decrement brick life
					--bricks[i][j].life;
					//mask  
					if (bricks[i][j].life == 0) {
						--BRICK_NUM;
						os_mut_wait(&mut_GLCD, 0xffff);
						GLCD_Bitmap(bricks[i][j].x_pos, bricks[i][j].y_pos, BRICKS_WIDTH, BRICKS_HEIGHT, (unsigned char*)bricks_bitmap_mask);
						os_mut_release(&mut_GLCD);
					}
					//flip x_velocity if the collision is with one of the sides
					if(((ball.x_pos == (BRICKS_WIDTH + bricks[i][j].x_pos)) && (ball.x_vel < 0)) || ((ball.x_pos + ball.diameter == bricks[i][j].x_pos) && (ball.x_vel > 0))){ //do we need to account for it hitting a corner? 
						os_mut_wait(&ball.mut,0xffff);		
						ball.x_vel = -1*ball.x_vel;
						os_mut_release(&ball.mut);
					
					} else if (((ball.y_pos == (BRICKS_HEIGHT + bricks[i][j].y_pos)) && (ball.y_vel < 0)) ||  ((ball.y_pos + ball.diameter) == (bricks[i][j].y_pos)) && (ball.y_vel > 0) ){
					//flip y-velocity if the collision is with the top of the paddle
						os_mut_wait(&ball.mut,0xffff);
						ball.y_vel = -1*ball.y_vel;
						os_mut_release(&ball.mut);
					}
				}
			}
		}
		// update graphics
		if(ball.x_prev != ball.x_pos && ball.y_prev != ball.y_pos){
			os_tsk_create_ex(update_graphics_task, 2, 0);
		}
		os_mut_wait(&ball.mut,0xffff);
		
		// update ball positions
		ball.x_prev = ball.x_pos;
		ball.y_prev = ball.y_pos;
		ball.x_pos = ball.x_pos + ball.x_vel;
		if (ball.y_pos > Y_MAX) {
			//lose a life, reset
			--LIVES;
			LED_Off(LIVES);
			if(LIVES!=0){
				wait_for_button();
			}
		} else {			
			ball.y_pos = ball.y_pos + ball.y_vel; 
		}
		
		os_mut_release(&ball.mut);

		
		if(LIVES==0 || BRICK_NUM == 0){	
			GAME_OVER = 1; 
			if(BRICK_NUM == 0){
				WIN = 1;
			} else {
				WIN = 0;
			}
			//show end game
		}
	
	os_tsk_delete_self();
	
}

__task void init_task(void){
	uint8_t i = 0;
	ad_avg = 0;
	ad_val = 0;
	ad_val_ = 0xFFFF;    
	GAME_OVER = 0;
	LIVES = 8;
	BRICK_NUM = BRICKS_X*BRICKS_Y;
	WIN = 0;
	
	LED_Init();
	paddle_init();
	ball_init();
	
	GLCD_Init();
	GLCD_Clear(W);
	
	for(i = 0; i < LIVES; i++){
		LED_On(i);
	}
	
	os_mut_init(&mut_GLCD);
	os_sem_init(&sem_ADC,1);
	//rit_init();
	os_tsk_create_ex(update_graphics_task, 2, 0);
	bricks_init();
	wait_for_button();
	while (GAME_OVER == 0){
		os_tsk_create_ex(start_ADC,2,0);
		os_tsk_create_ex(update_paddle_task, 2, 0);
		os_tsk_create_ex(update_ball_task, 2, 0);
	}
	//failure screen or win screen, 
	if(WIN == 1){
 //winner!!!!
	} else {
	//you lose
	}
	os_tsk_delete_self();
}

int main (void) {
	ADC_Init();
	button_init();
// 	printf("start\n");
	os_sys_init(init_task);
	while(1);
}

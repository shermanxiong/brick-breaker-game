/* File contains structs for game objects:
	- paddle
	- ball
	- bricks
*/

#include "GLCD.h"

#define BRICKS_X 3
#define BRICKS_Y 2
#define BRICKS_X_MIN 60
#define BRICKS_Y_MIN 60
// #define BRICKS_X_MAX 50
// #define BRICKS_Y_MAX 10
#define BRICKS_WIDTH 50
#define BRICKS_HEIGHT 20

#define PADDLE_X_MAX 50
#define PADDLE_Y_MAX 20
#define BALL_DIAMETER 11


typedef struct {
	unsigned int x_pos;
	unsigned int x_prev;
	unsigned int width;
	unsigned int height;
	unsigned short bitmap[PADDLE_X_MAX * PADDLE_Y_MAX];
	unsigned short bitmap_mask[PADDLE_X_MAX * PADDLE_Y_MAX];
	OS_MUT mut;
} paddle_t;

typedef struct {
	unsigned int x_prev;
	unsigned int y_prev; 
	unsigned int x_pos;
	unsigned int y_pos;
	signed int x_vel;
	signed int y_vel;
	unsigned int diameter;
	OS_MUT mut;
	unsigned short *bitmap;
	unsigned short *bitmap_mask;
} ball_t;

typedef struct {
	unsigned int x_pos;
	unsigned int y_pos;
	unsigned int life;
	unsigned short *bitmap;
	unsigned short *bitmap_mask;
// 	unsigned int width;
// 	unsigned int height;
// 	unsigned short bitmap[BRICKS_X_MAX * BRICKS_Y_MAX];
// 	unsigned short bitmap_mask[BRICKS_X_MAX * BRICKS_Y_MAX];
} brick_t;

typedef struct {
	brick_t brick_array [BRICKS_X] [BRICKS_Y];
} brick_state_t;

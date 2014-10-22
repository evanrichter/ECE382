#include <msp430g2553.h>
#include "lab4.h"

extern void init();
extern void initNokia();
extern void clearDisplay();
extern void drawBlock(unsigned char row, unsigned char col);
extern void drawCircle(unsigned char row, unsigned char col);

/*
 * Returns a pair of values, the first being position, the second velocity
 */
SignedInt8Pair move1d(sint8 position, sint8 velocity, sint8 upperbound) {
	SignedInt8Pair c;
	if (position + velocity <= upperbound) {
		if (position + velocity >= 0) {
			c.x = position + velocity;
			c.y = velocity;
		} else {
			c.x = 0;
			c.y = -velocity;
		}
	} else {
		c.x = upperbound;
		c.y = -velocity;
	}
	return c;
}



void main() {

	unsigned short cnt, cnt2;
	int8 paddle, inputDelay, button_press;
	SignedInt8Pair position, velocity, temp;

	// === Initialize system ================================================
	IFG1=0; /* clear interrupt flag1 */
	WDTCTL=WDTPW+WDTHOLD; /* stop WD */
	button_press = FALSE;

	init();
	initNokia();

	position.x=X_INIT;
	position.y=Y_INIT;
	velocity.x=DX_INIT;
	velocity.y=DY_INIT;
	paddle=PADDLE_INIT;
	inputDelay=0;

	while(TRUE) {
		clearDisplay();
		drawCircle(position.y,position.x); //draw ball
		cnt = 0;
		for(;cnt<=PADDLE_SIZE;cnt++) drawBlock(paddle+cnt,0); //draw paddle

		//figure out the new position and velocity of x direction
		temp = move1d(position.x, velocity.x, X_UPPER_BOUND);
		position.x = temp.x;
		velocity.x = temp.y;

		////figure out the new position and velocity of y direction
		temp = move1d(position.y, velocity.y, Y_UPPER_BOUND);
		position.y = temp.x;
		velocity.y = temp.y;

		//when ball hits left row, check collision
		if (position.x<=0) {
			if (position.y < paddle || position.y > paddle + PADDLE_SIZE){
				//game over, taunt player
				clearDisplay();
				drawBlock(1,4);
				drawBlock(2,4);
				drawBlock(3,4);
				drawBlock(4,4);
				drawBlock(4,5);
				drawBlock(4,6);
				while (TRUE);
			} else {
				//collision, rebound ball
				velocity.x = -velocity.x;
			}
		}
		cnt=0;
		for(;cnt<1000;cnt++){
			cnt2=0;
			for(;cnt2<95;cnt2++);
			if (button_press==FALSE){
				if (UP_BUTTON == 0 && paddle > 0){
					paddle--;
					button_press=TRUE;
				} else if (DOWN_BUTTON == 0
						   && (paddle+PADDLE_SIZE) < Y_UPPER_BOUND){
					paddle++;
					button_press=TRUE;
				}
			}
		}
		if (button_press==TRUE){
			if (inputDelay >= 2 ){
				inputDelay=0;
				button_press=FALSE;
			} else {
				inputDelay++;
			}
		}
	}
}

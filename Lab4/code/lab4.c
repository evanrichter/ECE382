#include <msp430g2553.h>
#include "lab4.h"

extern void init();
extern void initNokia();
extern void clearDisplay();
extern void drawBlock(unsigned char row, unsigned char col);

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

	unsigned char	button_press;
	unsigned short cnt;
	SignedInt8Pair position, velocity, temp;

	// === Initialize system ================================================
	IFG1=0; /* clear interrupt flag1 */
	WDTCTL=WDTPW+WDTHOLD; /* stop WD */
	button_press = FALSE;

	init();
	initNokia();
	clearDisplay();

	position.x=X_INIT;
	position.y=Y_INIT;
	velocity.x=DX_INIT;
	velocity.y=DY_INIT;
	drawBlock(position.y,position.x);


	while(1) {
		clearDisplay();
		drawBlock(position.y,position.x);
		//figure out the new position and velocity of x direction
		temp = move1d(position.x, velocity.x, X_UPPER_BOUND);
		position.x = temp.x;
		velocity.x = temp.y;

		////figure out the new position and velocity of y direction
		temp = move1d(position.y, velocity.y, Y_UPPER_BOUND);
		position.y = temp.x;
		velocity.y = temp.y;

		cnt=0;
		for(;cnt<65000;cnt++){
		}
/*
			if (UP_BUTTON == 0) {
				while(UP_BUTTON == 0);
				if (y>=1) y=y-1;
				button_press = TRUE;
			} else if (DOWN_BUTTON == 0) {
				while(DOWN_BUTTON == 0);
				if (y<=6) y=y+1;
				button_press = TRUE;
			} else if (LEFT_BUTTON == 0) {
				while(LEFT_BUTTON == 0);
				if (x>=1) x=x-1;
				button_press = TRUE;
			} else if (RIGHT_BUTTON == 0) {
				while(RIGHT_BUTTON == 0);
				if (x<=10) x=x+1;
				button_press = TRUE;
			}

			if (button_press) {
				button_press = FALSE;
				clearDisplay();
				drawBlock(y,x);
			}
*/
		}
}




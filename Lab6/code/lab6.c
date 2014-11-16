#include <msp430g2553.h>
#include "buttonmap.h"
#include "lab6.h"

int32	irPacket = 0;
int8	newIrPacket = FALSE;

//motor state
int16	rightPWM  = 0x3000;
int16	leftPWM = 0xD000;
sint8	rightDirection = FORWARD;
sint8	leftDirection = FORWARD;

sint8 accelerate(int16* currPWM, sint8 direction) {
	switch (direction) {
		case BACKWARD:
			if (*currPWM < PWMSTEPSIZE){
				*currPWM = PWMSTEPSIZE - *currPWM;
				direction = FORWARD;
			}
			else *currPWM -= PWMSTEPSIZE;
			break;
		case FORWARD:
			if (*currPWM < MAXPWM - PWMSTEPSIZE) *currPWM += PWMSTEPSIZE;
			else *currPWM = MAXPWM;
			break;
	}
	return direction;
}

sint8 decelerate(int16* currPWM, sint8 direction) {
	return -accelerate(currPWM, -direction);
}

// -----------------------------------------------------------------------
// Main loop: reads input from irPacket, directs PWM change as required
// -----------------------------------------------------------------------
void main(void) {

	initMSP430();		// Setup MSP to process IR and buttons

	while(1)  {
		if (newIrPacket) {
			switch (irPacket) {
				case ZER:
					//do stuff
					break;
				case ONE:
					//accelerate left motor
					leftDirection = accelerate(&leftPWM, leftDirection);
					break;
				case TWO:
					//accelerate both motors
					leftDirection = accelerate(&leftPWM, leftDirection);
					rightDirection = accelerate(&rightPWM, rightDirection);
					break;
				case THR:
					//accelerate right motor
					rightDirection = accelerate(&rightPWM, rightDirection);
					break;
				case FOU:
					//stop left motor
					leftPWM = 0;
					break;
				case FIV:
					//stop both motors
					leftPWM = 0;
					rightPWM = 0;
					break;
				case SIX:
					//stop right motor
					rightPWM = 0;
					break;
				case SEV:
					//decelerate left motor
					leftDirection = decelerate(&leftPWM, leftDirection);
					break;
				case EIG:
					//decelerate both motors
					leftDirection = decelerate(&leftPWM, leftDirection);
					rightDirection = decelerate(&rightPWM, rightDirection);
					break;
				case NIN:
					//decelerate right motor
					rightDirection = decelerate(&rightPWM, rightDirection);
					break;
			}
			newIrPacket = FALSE;
			irPacket = 0;
		}
	} // end infinite loop
} // end main


void initMSP430() {

	IFG1=0;
	WDTCTL=WDTPW+WDTHOLD;

	BCSCTL1 = CALBC1_8MHZ;
	DCOCTL = CALDCO_8MHZ;

	P2SEL  &= ~BIT6;					// Setup P2.6 for IR receiver
	P2SEL2 &= ~BIT6;
	P2DIR &= ~BIT6;
	P2IFG &= ~BIT6;
	P2IE  |= BIT6;
	HIGH_2_LOW;


	P1DIR = BIT0 | BIT6;		// Enable updates to the LED
	P1OUT = (BIT0 | BIT6);		// An turn the LED on

	TA0CCR0 = 10000;			// create a 10mS roll-over period for Timer_A
	TA0CTL &= ~TAIFG;
	TA0CTL = TASSEL_2 | ID_3 | MC_1 | TAIE;

	TA1CCR0 = 2015;
	TA1CTL &= ~TAIFG;
	TA1CTL = TASSEL_2 | ID_0 | MC_1 | TAIE;

	_enable_interrupt();
}


#pragma vector = PORT2_VECTOR
__interrupt void pinChange (void) {

	int8	pin;
	int16	pulseDuration;

	if (IR_PIN)		pin=1;	else pin=0;

	switch (pin) {
		case 0:
			pulseDuration = TA0R;
			if      (pulseDuration < minLogic0Pulse);
			else if (pulseDuration < maxLogic0Pulse){
				irPacket = irPacket<<1;
				irPacket = irPacket & 0xFFFFFFE;
			}
			else if (pulseDuration < minLogic1Pulse);
			else if (pulseDuration < maxLogic1Pulse){
				irPacket = irPacket<<1;
				irPacket = irPacket | 0x1;
			}
			else if (pulseDuration < minStartPulse);
			else if (pulseDuration < maxStartPulse){
				irPacket = 0;
			}

			TA0CTL = 0;
			LOW_2_HIGH;
			break;

		case 1:
			TA0R = 0x0000;
			TA0CTL = ID_3 | TASSEL_2 | MC_1 | TAIE;
			HIGH_2_LOW;
			break;
	}

	P2IFG &= ~BIT6;

}


#pragma vector = TIMER0_A1_VECTOR		//Timer_A0 is for IR packet reading
#pragma vector = TIMER1_A1_VECTOR		//Timer_A1 is for PWM management
__interrupt void timerOverflow (void) {

	static int8 samePacketCounter = 0;
	static int32 prevPacket = 0;

	if (TA0CTL | TAIE == TRUE) { // Timer_A0 overflow
		if (irPacket != prevPacket | samePacketCounter > 100){
			newIrPacket = TRUE;
			prevPacket = irPacket;
		} else {
			samePacketCounter++;
		}
		TA0CTL = 0;
	}

	else if (TA1CTL | TAIE == TRUE) { // Timer_A1 overflow
		static int8 prev = OFF;
		int16 smallerPWM;
		int16 largerPWM;
		int16 slowerMotor;
		int16 fasterMotor;

		if (rightPWM < leftPWM) {
			smallerPWM = rightPWM;
			slowerMotor = RIGHTMOTOR;
		} else {
			smallerPWM = leftPWM;
			slowerMotor = LEFTMOTOR;
		}

		switch (prev) {
		case OFF:
			//turn on both motors
			TA1CCR0 = smallerPWM;
			prev = SHORTER;
			break;
		case SHORTER:
			//turn off slower motor
			TA1CCR0 = largerPWM - smallerPWM;
			prev = DIFFERENCE;
			break;
		case DIFFERENCE:
			//turn off faster motor
			TA1CCR0 = MAXPWM - largerPWM;
			prev = OFF;
			break;
		}

		TA1CTL &= ~TAIFG;
	}
}

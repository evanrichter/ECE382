#include <msp430g2553.h>
#include "buttonmap.h"
#include "lab6.h"
#include "lab7.h"

int32	irPacket = 0;
int8	newIrPacket = FALSE;

//motor state
sint8	rightMotor = 0;
sint8	leftMotor = 0;

//reported sensor distance
int16	leftDistance = 0;
int16	centerDistance = 0;
int16	rightDistance = 0;

//calibration values for distances
int16	rightTarget = 0;
int16	centerTarget = 0;
int16	rightNoWall = 0;

int8	stage = LEFT_L;

sint8 accelerate(sint8 velocity) {
	sint8 newVelocity;
	if (velocity < -MINVELOCITY) newVelocity = velocity + VELOCITYSTEP;
	else if (velocity == -MINVELOCITY) newVelocity = 0;
	else if (velocity == 0) newVelocity = MINVELOCITY;
	else if (velocity < MAXVELOCITY && velocity > 0) newVelocity = velocity + VELOCITYSTEP;
	else newVelocity = MAXVELOCITY;
	return newVelocity;
}

sint8 decelerate(sint8 velocity) {
	sint8 newVelocity;
	if (velocity > MINVELOCITY) newVelocity = velocity - VELOCITYSTEP;
	else if (velocity == MINVELOCITY) newVelocity = 0;
	else if (velocity == 0) newVelocity = -MINVELOCITY;
	else if (velocity > -MAXVELOCITY && velocity <= MINVELOCITY) newVelocity = velocity - VELOCITYSTEP;
	else newVelocity = -MAXVELOCITY;
	return newVelocity;
}

void updatePWM() {
	//set PWM mode for left
	if (leftMotor < 0) {
		TA1CCTL1 = OUTMOD_3;		//inverted PWM mode
		P2OUT |= BIT1;				//direction set to 1
		P2OUT |= BIT0;				//set enable
		TA1CCR1 = -(leftMotor * PWMSCALE);
	} else if (leftMotor == 0) {
		P2OUT &= ~BIT0;				//simply turn off enable
	} else {
		TA1CCTL1 = OUTMOD_7;		//normal PWM mode
		P2OUT &= ~BIT1;				//direction set to 0
		P2OUT |= BIT0;				//set enable
		TA1CCR1 = leftMotor * PWMSCALE;
	}

	//same for right
	if (rightMotor < 0) {
		TA1CCTL2 = OUTMOD_3;		//inverted PWM mode
		P2OUT |= BIT5;				//direction set to 1
		P2OUT |= BIT3;				//set enable
		TA1CCR2 = -(rightMotor * PWMSCALE);
	} else if (rightMotor == 0) {
		P2OUT &= ~BIT3;				//simply turn off enable
	} else {
		TA1CCTL2 = OUTMOD_7;		//normal PWM mode
		P2OUT &= ~BIT5;				//direction set to 0
		P2OUT |= BIT3;				//set enable
		TA1CCR2 = rightMotor * PWMSCALE;
	}
}

void getSensors(void){
	int16 leftSum = 0;
	int16 centerSum = 0;
	int16 rightSum = 0;

	int8 i = 0;
	for (;i<16;i++){
		ADC10CTL0 = 0;
		ADC10CTL1 = INCH_2 | ADC10DIV_3 ;
		ADC10AE0 = BIT2;
		ADC10CTL0 = SREF_0 | ADC10SHT_3 | ADC10ON | ENC;
		ADC10CTL0 |= ADC10SC;
		while(ADC10CTL1 & ADC10BUSY);
		leftSum += ADC10MEM;

		ADC10CTL0 = 0;
		ADC10CTL1 = INCH_3 | ADC10DIV_3 ;
		ADC10AE0 = BIT3;
		ADC10CTL0 = SREF_0 | ADC10SHT_3 | ADC10ON | ENC;
		ADC10CTL0 |= ADC10SC;
		while(ADC10CTL1 & ADC10BUSY);
		centerSum += ADC10MEM;

		ADC10CTL0 = 0;
		ADC10CTL1 = INCH_4 | ADC10DIV_3 ;
		ADC10AE0 = BIT4;
		ADC10CTL0 = SREF_0 | ADC10SHT_3 | ADC10ON | ENC;
		ADC10CTL0 |= ADC10SC;
		while(ADC10CTL1 & ADC10BUSY);
		rightSum += ADC10MEM;
	}

	leftDistance = leftSum >> 4;
	centerDistance = centerSum >> 4;
	rightDistance = rightSum >> 4;
}

void doombaJustWaitaLittle(void) {
	__delay_cycles(600000);
}

void doombaJustWait(void) {
	doombaJustWaitaLittle();
	doombaJustWaitaLittle();
}

void doombaStop(void) {
	rightMotor = 0;
	leftMotor = 0;
	updatePWM();
	doombaJustWaitaLittle();
}

void doombaBackup(void) {
	rightMotor = -16;
	leftMotor = -16;
	updatePWM();
	doombaJustWaitaLittle();
}

void doombaForward(void) {
	rightMotor = 16;
	leftMotor = 16;
	updatePWM();
	doombaJustWait();
}

void doombaForwardThisMuch(int8 k) {
	for (;k>0;k--){
		doombaForward();
		doombaStop();
		doombaJustWait();
	}
}

void doombaPivotLeftThisMuch(int8 k) {
	for(;k>0;k--){
		rightMotor = MAXVELOCITY;
		leftMotor = -MAXVELOCITY;
		updatePWM();
		__delay_cycles(200000);
		doombaStop();
		doombaJustWaitaLittle();
	}
}

void doombaPivotRightThisMuch(int8 k) {
	for(;k>0;k--){
		rightMotor = -MAXVELOCITY;
		leftMotor = MAXVELOCITY;
		updatePWM();
		__delay_cycles(200000);
		doombaStop();
		doombaJustWaitaLittle();
	}
}

void doombaPivotLeft90Stutter(void) {
	doombaPivotLeftThisMuch(35);
}

void doombaPivotRight90Stutter(void) {
	doombaPivotRightThisMuch(34);
}

void doombaLeft(void) {
	rightMotor = 17;
	leftMotor = 16;
	updatePWM();
	doombaJustWait();
}

void doombaRight(void) {
	rightMotor = 16;
	leftMotor = 17;
	updatePWM();
	doombaJustWait();
}

void blinkLED(int16 mask) {
	int8 i = 0;
	for(;i<15;i++){
		P1OUT &= ~mask;
		__delay_cycles(200000);
		P1OUT |= mask;
		__delay_cycles(200000);
	}
}
void testWall(void){
	getSensors();
	if (centerDistance > centerTarget ) {
		P1OUT |= BIT0;
	} else {
		P1OUT &= ~BIT0;
	}

	if (rightDistance > rightTarget + R_THR) {
		P1OUT &= ~BIT6;
	} else if (rightDistance > rightTarget - R_THR) {
		P1OUT |= BIT6;
	} else if (rightDistance < rightNoWall) {
		blinkLED(BIT6);
	} else {
		P1OUT &= ~BIT6;
	}
}

void main(void) {

	initMSP430();		// Setup MSP to process IR and buttons

	while (BUTTON);
	while (!BUTTON);
	getSensors();
	rightTarget = rightDistance;	//calibrate right wall
	blinkLED(BIT6);
	while (BUTTON) testWall();
	while (!BUTTON) testWall();
	getSensors();
	centerTarget = centerDistance + 0x25;	//calibrate front wall
	blinkLED(BIT0);
	while (BUTTON) testWall();
	while (!BUTTON) testWall();
	getSensors();
	rightNoWall = rightDistance + R_THR;	//calibrate front wall
	blinkLED(BIT6);

	while (BUTTON) testWall();
	while (!BUTTON) testWall();

	while(1)  {
		getSensors();
		switch (stage) {
		case LEFT_L:
			//follow right wall until head on
			//90* left turn
			testWall();
			if (centerDistance > centerTarget ) {
				doombaStop();
				doombaPivotLeft90Stutter();
				doombaStop();
			} else if (rightDistance > rightTarget + R_THR) {
				//too close!
				//doombaLeft();
				doombaPivotLeftThisMuch(6);
				doombaForward();
				doombaPivotRightThisMuch(2);
			} else if (rightDistance > rightTarget - R_THR) {
				//continue on
				doombaForward();
			} else if (rightDistance < rightNoWall) {
				stage = HORSESHOE;
				break;
			} else {
				//too far!
				//doombaRight();
				doombaPivotRightThisMuch(5);
				doombaForward();
				doombaPivotLeftThisMuch(3);
			}

			doombaStop();
			doombaJustWaitaLittle();
			//doombaJustWait();
			break;
		case HORSESHOE:
			//go forward 1 length, 90* right turn, forward 1.5 lengths
			doombaForwardThisMuch(5);
			doombaPivotRight90Stutter();
			doombaForwardThisMuch(12);
			doombaPivotRight90Stutter();
			doombaForwardThisMuch(9);

			stage = OPENSPACE;
			break;
		case OPENSPACE:
			//pivot left if close wall
			//follow wall until head on
			//90* left
			//follow wall
			testWall();
			if (centerDistance > centerTarget ) {
				doombaStop();
				doombaPivotLeft90Stutter();
				doombaStop();
			} else if (rightDistance > rightTarget + R_THR) {
				//too close!
				//doombaLeft();
				doombaPivotLeftThisMuch(6);
				doombaForward();
				doombaPivotRightThisMuch(2);
			} else if (rightDistance > rightTarget - R_THR) {
				//continue on
				doombaForward();
			} else {
				//too far!
				//doombaRight();
				doombaPivotRightThisMuch(5);
				doombaForward();
				doombaPivotLeftThisMuch(3);
			}

			doombaStop();
			doombaJustWaitaLittle();
			break;
		}
	}
}


void initMSP430() {
	IFG1=0;
	WDTCTL=WDTPW+WDTHOLD;

	BCSCTL1 = CALBC1_8MHZ;
	DCOCTL = CALDCO_8MHZ;

	//P1.7 button as input (big red button
	P1DIR &= ~BIT7;
	P1REN &= ~BIT7;
	P1OUT |= BIT7;

	P1DIR |= BIT0 | BIT6;
	P1DIR &= ~(BIT5 | BIT3 | BIT4);

	//P2.0, P2.1, P2.3, P2.5 are GPIO for left/right direction/enable
	P2DIR |=   BIT0 | BIT1 | BIT3 | BIT5;
	P2OUT &= ~(BIT0 | BIT1 | BIT3 | BIT5);		//turn them off to start

    //P2.2 and P2.4 used for PWM control
    P2DIR |= BIT2 | BIT4;
    P2SEL |= BIT2 | BIT4;		//primary peripheral control mode for direct PWM output

    //Timer A1 for PWM stuff
    TA1CTL &= ~TAIFG;
    TA1CTL = TASSEL_2 | ID_3 | MC_1 | TAIE;
    TA1CCR0 = PWMPERIOD;
    TA1CCR1 = 1 * PWMSCALE;			//pwmLeft
    TA1CCR2 = 1 * PWMSCALE;			//pwmRight
    TA1CCTL1 = OUTMOD_3;
    TA1CCTL2 = OUTMOD_3;

}

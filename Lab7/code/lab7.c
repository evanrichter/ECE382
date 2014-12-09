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
int16	leftDistance = L_8IN;
int16	centerDistance = C_8IN;
int16	rightDistance = R_8IN;

int16	rightTarget = 0;
int16	centerTarget = 0;

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
	__delay_cycles(400000);
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
	rightMotor = -14;
	leftMotor = -14;
	updatePWM();
	doombaJustWaitaLittle();
}

void doombaForward(void) {
	rightMotor = 13;
	leftMotor = 13;
	updatePWM();
	doombaJustWaitaLittle();
}

void doombaPivotLeft90(void) {
	rightMotor = 17;
	leftMotor = -17;
	updatePWM();
	__delay_cycles(2000000);
}

void doombaPivotRight90(void) {
	rightMotor = -17;
	leftMotor = 17;
	updatePWM();
	__delay_cycles(2000000);
}

void doombaPivotLeft90Stutter(void) {
	int8 i = 0;
	for(;i<35;i++){
		rightMotor = MAXVELOCITY;
		leftMotor = -MAXVELOCITY;
		updatePWM();
		__delay_cycles(200000);
		doombaStop();
		doombaJustWaitaLittle();
	}
}

void doombaPivotRight90Stutter(void) {
	int8 i = 0;
	for(;i<34;i++){
		rightMotor = -MAXVELOCITY;
		leftMotor = MAXVELOCITY;
		updatePWM();
		__delay_cycles(200000);
		doombaStop();
		doombaJustWaitaLittle();
	}
}

void main(void) {

	initMSP430();		// Setup MSP to process IR and buttons

	while (BUTTON);
	while (!BUTTON);
	getSensors();
	rightTarget = rightDistance;
	while (BUTTON);
	while (!BUTTON);
	getSensors();
	centerTarget = centerDistance;

	while (BUTTON);
	while (!BUTTON);

	while(1)  {
		//turning calibration
		doombaPivotLeft90Stutter();
		doombaStop();
		doombaJustWait();
		doombaJustWait();
		doombaJustWait();
		doombaJustWait();
		doombaJustWait();
		doombaJustWait();
		doombaJustWait();
		doombaJustWait();

		doombaPivotRight90Stutter();
		doombaStop();
		doombaJustWait();
		doombaJustWait();
		doombaJustWait();
		doombaJustWait();
		doombaJustWait();
		doombaJustWait();
		doombaJustWait();
		doombaJustWait();
		doombaJustWait();
		/*getSensors();

		switch (stage) {
		case LEFT_L:
			//follow right wall until head on
			//90* left turn
			break;
		case HORSESHOE:
			//when sensor reads "too far" go forward 1 length, 90* right turn, forward 1.5 lengths
			break;
		case OPENSPACE:
			//pivot left if close wall
			//follow wall until head on
			//90* left
			//follow wall
			break;
		}
		if (centerDistance > centerTarget ) {
			P1OUT |= BIT0;
			doombaStop();
			doombaPivotLeft();
			doombaStop();
		} else if (rightDistance > rightTarget + R_THR) {
			P1OUT &= ~BIT0;
			P1OUT |= BIT6;

			//too close!
			doombaLeft();

		} else if (rightDistance > rightTarget - R_THR) {
			P1OUT &= ~BIT0;
			P1OUT &= ~BIT6;

			//continue on
			doombaForward();
		} else {
			P1OUT &= ~BIT0;
			P1OUT |= BIT6;

			//too far!
			doombaRight();
		} */

		doombaStop();
		doombaJustWait();
		doombaJustWait();
	}
}


void initMSP430() {
	IFG1=0;
	WDTCTL=WDTPW+WDTHOLD;

	BCSCTL1 = CALBC1_8MHZ;
	DCOCTL = CALDCO_8MHZ;

	//P1.3 button as input
	P1DIR &= ~BIT3;
	P1REN |= BIT3;
	P1OUT |= BIT3;

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

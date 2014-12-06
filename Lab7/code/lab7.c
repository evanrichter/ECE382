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

void doombaStop(void) {
	rightMotor = 0;
	leftMotor = 0;
	updatePWM();
}

void doombaBackup(void) {
	rightMotor = -MAXVELOCITY;
	leftMotor = -MAXVELOCITY;
	updatePWM();
}

void doombaForward(void) {
	rightMotor = MAXVELOCITY;
	leftMotor = MAXVELOCITY;
	updatePWM();
}

void doombaPivotLeft(void) {
	rightMotor = MAXVELOCITY;
	leftMotor = -MAXVELOCITY;
	updatePWM();
}

void doombaLeft(void) {
	rightMotor = 16;
	leftMotor = 15;
	updatePWM();
}

void doombaRight(void) {
	rightMotor = 15;
	leftMotor = 16;
	updatePWM();
}

void doombaJustWait(void) {
	__delay_cycles(5000000);
}

void doombaJustWaitaLittle(void) {
	__delay_cycles(500000);
}

void main(void) {

	initMSP430();		// Setup MSP to process IR and buttons

	while(1)  {
		getSensors();

		if (centerDistance > C_1IN ) {
			P1OUT |= BIT0;

			doombaStop();
			doombaJustWait();

			doombaBackup();
			doombaJustWaitaLittle();

			doombaPivotLeft();
			doombaJustWaitaLittle();

			doombaStop();
			doombaJustWait();
		} else if (rightDistance > 0x275) {
			P1OUT &= ~BIT0;
			P1OUT |= BIT6;

			//too close!
			doombaLeft();
			doombaJustWaitaLittle();

			doombaForward();
			doombaJustWaitaLittle();
		} else if (rightDistance > R_6IN) {
			P1OUT &= ~BIT0;
			P1OUT &= ~BIT6;

			//continue on
			doombaForward();
			doombaJustWaitaLittle();
		} else {
			P1OUT &= ~BIT0;
			P1OUT &= ~BIT6;

			//too far!
			doombaRight();
			doombaJustWaitaLittle();
		}
	}
}


void initMSP430() {
	IFG1=0;
	WDTCTL=WDTPW+WDTHOLD;

	BCSCTL1 = CALBC1_8MHZ;
	DCOCTL = CALDCO_8MHZ;

	P1DIR = BIT0 | BIT6;
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

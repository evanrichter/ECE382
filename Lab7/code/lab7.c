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

// -----------------------------------------------------------------------
// Main loop: reads input from irPacket, directs PWM change as required
// -----------------------------------------------------------------------
void main(void) {

	initMSP430();		// Setup MSP to process IR and buttons

	while(1)  {
		if (newIrPacket) {
			switch (irPacket) {
				case ZERO:
					//do stuff. how about a donut?
					leftMotor = -MAXVELOCITY;
					rightMotor = MAXVELOCITY;
					break;
				case ONE:
					//accelerate left motor
					leftMotor = accelerate(leftMotor);
					break;
				case TWO:
					//accelerate both motors
					leftMotor = accelerate(leftMotor);
					rightMotor = accelerate(rightMotor);
					break;
				case THREE:
					//accelerate right motor
					rightMotor = accelerate(rightMotor);
					break;
				case FOUR:
					//stop left motor
					leftMotor = 0;
					break;
				case FIVE:
					//stop both motors
					leftMotor = 0;
					rightMotor = 0;
					break;
				case SIX:
					//stop right motor
					rightMotor = 0;
					break;
				case SEVEN:
					//decelerate left motor
					leftMotor = decelerate(leftMotor);
					break;
				case EIGHT:
					//decelerate both motors
					leftMotor = decelerate(leftMotor);
					rightMotor = decelerate(rightMotor);
					break;
				case NINE:
					//decelerate right motor
					rightMotor = decelerate(rightMotor);
					break;
				case PLY:
					rightMotor = MAXVELOCITY;
					leftMotor = MAXVELOCITY;
					updatePWM();
					_delay_cycles(1500000);
					rightMotor = 0;
					leftMotor = 0;
					break;
				case STP:
					rightMotor = -MAXVELOCITY;
					leftMotor = -MAXVELOCITY;
					updatePWM();
					_delay_cycles(2500000);
					rightMotor = 0;
					leftMotor = 0;
					break;
			}

			updatePWM();

			newIrPacket = FALSE;
			irPacket = 0;
		} else {
			if (rightDistance > R_4IN) P1OUT |= BIT0;
			else						P1OUT &= ~BIT0;
		}
	}
}


void initMSP430() {
	IFG1=0;
	WDTCTL=WDTPW+WDTHOLD;

	BCSCTL1 = CALBC1_8MHZ;
	DCOCTL = CALDCO_8MHZ;

	P1DIR = BIT0;

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

    //Pin 2.6 for IR
    P2SEL  &= ~BIT6;
	P2SEL2 &= ~BIT6;
	P2DIR &= ~BIT6;
	P2IFG &= ~BIT6;
	P2IE  |= BIT6;
	HIGH_2_LOW;

	//IR input timer
	TA0CCR0 = 10000;
	TA0CTL &= ~TAIFG;
	TA0CTL = TASSEL_2 | ID_3 | MC_1 | TAIE;

	// ADC setup
	ADC10CTL0 = 0;											// Turn off ADC subsystem
	ADC10CTL1 = INCH_4 | ADC10DIV_3 ;						// Channel 4, ADC10CLK/4
	ADC10AE0 = BIT4;		 								// Make P1.4 analog input
	ADC10CTL0 = SREF_0 | ADC10SHT_3 | ADC10ON | ENC;		// Vcc & Vss as reference
	ADC10CTL0 |= ADC10SC;									// Start a conversion
	while(ADC10CTL1 & ADC10BUSY);

	_enable_interrupt();
	_BIS_SR(GIE);			// suggested by http://homepages.ius.edu/RWISMAN/C335/HTML/msp430Interrupts.HTM

}


#pragma vector = PORT2_VECTOR
__interrupt void pinChange (void) {

	int8	pin;
	int16	pulseDuration;

	if (IR_PIN)		pin=1;	else pin=0;
	if (!newIrPacket){
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
	}

	P2IFG &= ~BIT6;
}


#pragma vector = TIMER0_A1_VECTOR		//Timer_A0 is for IR packet reading
__interrupt void timerOverflow (void) {

	static int8 samePacketCounter = 0;
	static int32 prevPacket = 0;

	if (TA0CTL | TAIE == TRUE) { // Timer_A0 overflow
		if (irPacket != prevPacket | samePacketCounter > 0){
			newIrPacket = TRUE;
			prevPacket = irPacket;
		} else {
			samePacketCounter++;
		}
	}

	TA0CTL = 0;
}
#pragma vector = ADC10_VECTOR		//ADC stuff
__interrupt void pollADC (void) {
	_BIC_SR(CPUOFF);		// suggested by http://homepages.ius.edu/RWISMAN/C335/HTML/msp430Interrupts.HTM


	static int8 nextSensor = RIGHT;

	//ADC10CTL0 = 0;		// Turn off ADC subsystem
	ADC10CTL0 &= ~ENC;

	switch(nextSensor) {
	case LEFT:
		rightDistance = ADC10MEM;			// record the distance
		ADC10CTL1 = INCH_2 | ADC10DIV_3 ;	// set up next channel for recording
		ADC10AE0 = BIT2;
		nextSensor = CENTER;				// set new state
		break;
	case CENTER:
		leftDistance = ADC10MEM;
		ADC10CTL1 = INCH_3 | ADC10DIV_3 ;
		ADC10AE0 = BIT3;
		nextSensor = RIGHT;
		break;
	case RIGHT:
		centerDistance = ADC10MEM;
		ADC10CTL1 = INCH_4 | ADC10DIV_3 ;
		ADC10AE0 = BIT4;
		nextSensor = LEFT;
		break;
	}

	ADC10CTL0 |= ENC;
	//ADC10CTL0 = SREF_0 | ADC10SHT_3 | ADC10ON | ENC;
	ADC10CTL0 |= ADC10SC;		// Start the conversion


	ADC10CTL0 &= ~ADC10IFG;
}

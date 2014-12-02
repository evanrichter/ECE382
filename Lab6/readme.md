Lab 6 - PWM & Robot Movement
===

Objective
---
This lab is to serve as an introduction to the MSP430's PWM (pulse-width modulation) capabilities. The application of this knowledge will be two PWM signals driving two independant motors on the issued robots.

Prelab
---
Consider your hardware (timer subsystems, chip pinout, etc.) and how you will use it to achieve robot control. Which pins will output which signals you need? Which side of the motor will you attach these signals to? How will you use these signals to achieve forward / back / left / right movement? Spend some time here, as these decisions will dictate much of how difficult this lab is for you.

Timer A1 will be my PWM control because of its built-in ability to drive P2.2 with TA1CCR1 and P2.4 with TA1CCR2. Register TA1CCR0 controls the period for both. A PWM signal will be connected to one lead of each motor, and the other lead will be connected to GPIO pins that will designate direction of motion. The enable pin on the SN754410 motor driver chip will also be hooked up to a GPIO pin on the MSP430, and will turn the motor on or off.

| MSP430 Pin |        Function       |
|:----------:|:---------------------:|
|    P2.0    |   Left Motor Enable   |
|    P2.1    |  Left Motor Direction |
|    P2.2    |     Left Motor PWM    |
|    P2.3    |   Right Motor Enable  |
|    P2.4    |    Right Motor PWM    |
|    P2.5    | Right Motor Direction |

The following whiteboard drawing, courtesy of Dr. Coulston, shows my general plan for motor hookup. This will be done twice, one for the right motor and one for the left. The capacitors are to help reduce electrical noise from the motors.

![](./images/motor_driver_plan.png "Motor Driver Plan")

This diagram is of the SN754410, the motor driver.

![](./images/motor_driver_pin_diagram.png "Motor Driver Schematic")

### Control Scheme

To interface with my robot, I will utilize IR input from a remote control according to the control scheme below. The IR decoding subsystem uses Timer A0, so it won't interfere with PWM control.

![] (./images/control_plan.png "Control Scheme")

The following switch statement will provide this type of movement.
```
switch (irPacket) {
	case ZERO:
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
	case THREE:
		//accelerate right motor
		rightDirection = accelerate(&rightPWM, rightDirection);
		break;
	case FOUR:
		//stop left motor
		leftPWM = 0;
		break;
	case FIVE:
		//stop both motors
		leftPWM = 0;
		rightPWM = 0;
		break;
	case SIX:
		//stop right motor
		rightPWM = 0;
		break;
	case SEVEN:
		//decelerate left motor
		leftDirection = decelerate(&leftPWM, leftDirection);
		break;
	case EIGHT:
		//decelerate both motors
		leftDirection = decelerate(&leftPWM, leftDirection);
		rightDirection = decelerate(&rightPWM, rightDirection);
		break;
	case NINE:
		//decelerate right motor
		rightDirection = decelerate(&rightPWM, rightDirection);
		break;
}
```

Achieving Functionality
---
### Wiring
The following image documents how I wired the protoboard on the top tier of the robot. The seemingly excessive amount of capacitors were necessary for reducing noise produced by the motion of the DC motors. More on that in the debugging section.
![](./images/wiring3.jpg "Wiring")

### Critical Code Walkthrough
In this section I will comment on various sections of code that are critical to achieving functionality in this lab.

#### Determining A New Motor Velocity
The following code determines the new velocity of the motor, based on what the user inputs using the remote control. I found through testing that a linear increase in motor velocity is only useful after a certain minimum threshold. If the motor PWM is held lower than that useful threshold, it will just sit and whine, as the motor cannot provide enough torque needed for motion.

```
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
```
#### Pinout and PWM
The next block of code shows the `updatePWM()` method. This is critical for translating the signed velocity values of each motor and then applying the correct pinout modes. This is a main section where I implemented the pin function definition table above into code. This code is called anytime the PWM state changes (after user input).

```
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
```
#### Capturing User Input
In the main thread, an infinite loop polls the `newIrPacket` flag. Upon receiving a new, complete packet, a switch statement decides what to do. This is where I implemented the control scheme in my prelab above. `ZERO`, `ONE`, `TWO`, etc. are the IR codes that the remote control sends, and they are defined in `buttonmap.h`.
```
while(1)  {
	if (newIrPacket) {
		switch (irPacket) {
			case ZERO:
				//do stuff. how about a donut?
				leftMotor = -MAXVELOCITY;
				rightMotor = MAXVELOCITY;
				break;
			case ONE:
				//do other stuff
				break;
		}
		updatePWM();
		newIrPacket = FALSE;
		irPacket = 0;
	}
}
```

### Debugging
In this lab, I learned a great lesson in debugging. Over the weekend I had time to work on code but did not have access to the robot to test the code. I did what I could by adjusting the PWM of the red and green LEDs independently, with good results. However, the overall result was that I typed too much code to debug at once. There were too many variables. I resolved the problem by commenting out whole blocks of code with `/*` and `*/`, until I had one section to focus on.

Another point of frustration was motor noise due to the motion of the wheels. I saw on the oscilloscope noise that would interfere with the IR sensor. It was strong enough to pull the input pin high or low at random. There was a consistent 1.5kHz pulse to 0 then back to 1.

//-----------------------------------------------------------------
// Page 76 : MSP430 Optimizing C/C++ Compiler v 4.3 User's Guide
//-----------------------------------------------------------------
typedef		unsigned char		int8;
typedef		signed char			sint8;
typedef		unsigned short		int16;
typedef		signed short		sint16;
typedef		unsigned long		int32;
typedef		unsigned long long	int64;

#define		TRUE			1
#define		FALSE			0

//-----------------------------------------------------------------
// Function prototypes found in lab5.c
//-----------------------------------------------------------------
void initMSP430();
__interrupt void pinChange (void);
__interrupt void timerOverflow (void);


//-----------------------------------------------------------------
// Each PxIES bit selects the interrupt edge for the corresponding I/O pin.
//	Bit = 0: The PxIFGx flag is set with a low-to-high transition
//	Bit = 1: The PxIFGx flag is set with a high-to-low transition
//-----------------------------------------------------------------

#define		IR_PIN			(P2IN & BIT6)
#define		HIGH_2_LOW		P2IES |= BIT6
#define		LOW_2_HIGH		P2IES &= ~BIT6


#define		RIGHTMOTOR		BIT6
#define		LEFTMOTOR		BIT0

#define		FORWARD			1
#define		BACKWARD		-1

#define		PWMSTEPSIZE		0x1000
#define		MAXPWM			0xFFFF

//------------------------------------------
// Different states for dual PWM management
//------------------------------------------
#define		OFF				0
#define		SHORTER			1
#define		DIFFERENCE		2

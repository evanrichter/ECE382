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


#define		IR_PIN			(P2IN & BIT6)
#define		HIGH_2_LOW		P2IES |= BIT6
#define		LOW_2_HIGH		P2IES &= ~BIT6

#define		PWMPERIOD		PWMSCALE * MAXVELOCITY * 1.1
#define		PWMSCALE		0xFF
#define		VELOCITYSTEP	1
#define		MAXVELOCITY		24
#define		MINVELOCITY		12		//this should negate the noise problem on IR

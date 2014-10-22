#define		TRUE			1
#define		FALSE			0
#define		UP_BUTTON		(P2IN & BIT5)
#define		DOWN_BUTTON		(P2IN & BIT4)
#define		AUX_BUTTON		(P2IN & BIT3)
#define		LEFT_BUTTON		(P2IN & BIT2)
#define		RIGHT_BUTTON	(P2IN & BIT1)

#define		X_INIT			1
#define		Y_INIT			1
#define		DX_INIT			1
#define		DY_INIT			1
#define		PADDLE_INIT		1

#define		X_UPPER_BOUND	11
#define		Y_UPPER_BOUND	7
#define		PADDLE_SIZE		3

typedef unsigned char int8;
typedef signed char sint8;

typedef struct SignedInt8Pair_struct {
   sint8 x;
   sint8 y;
} SignedInt8Pair;

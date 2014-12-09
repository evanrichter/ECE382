__interrupt void pollADC (void);

#define		LEFT	0
#define		CENTER	1
#define		RIGHT	2

#define		L_1IN	0x23a
#define		L_2IN	0x199
#define		L_4IN	0x128
#define		L_6IN	0x106
#define		L_8IN	0x0fc

#define		C_1IN	0x355
#define		C_2IN	0x258
#define		C_4IN	0x192
#define		C_6IN	0x169
#define		C_8IN	0x136

#define		R_1IN	0x275
#define		R_2IN	0x188
#define		R_4IN	0x111
#define		R_6IN	0x0d7
#define		R_8IN	0x0cc
#define		R_THR	0x060

#define		BUTTON	(P1IN & BIT3)

#define		LEFT_L		1
#define		HORSESHOE	2
#define		OPENSPACE	3

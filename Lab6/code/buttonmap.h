// IR remote control codes for my unbranded remote

#define		averageLogic0Pulse	580
#define		averageLogic1Pulse	1610
#define		averageStartPulse	4400
#define		minLogic0Pulse		averageLogic0Pulse - 100
#define		maxLogic0Pulse		averageLogic0Pulse + 100
#define		minLogic1Pulse		averageLogic1Pulse - 100
#define		maxLogic1Pulse		averageLogic1Pulse + 100
#define		minStartPulse		averageStartPulse - 100
#define		maxStartPulse		averageStartPulse + 100

#define		PWR		0x01dec837

#define		ZERO	0x01de00ff
#define		ONE		0x01de807f
#define		TWO		0x01de40bf
#define		THREE	0x01dec03f
#define		FOUR	0x01dea03f
#define		FIVE	0x01ded03f
#define		SIX		0x01dee03f
#define		SEVEN	0x01de503f
#define		EIGHT	0x01de403f
#define		NINE	0x01de303f

#define		FFW		0x01de18e7
#define		RRW		0x01de9867
#define		PLY		0x01de6897
#define		STP		0x01dee817

// IR remote control codes for my unbranded remote

#define		averageLogic0Pulse	580
#define		averageLogic1Pulse	1610
#define		averageStartPulse	4400
#define		minLogic0Pulse		averageLogic0Pulse - 200
#define		maxLogic0Pulse		averageLogic0Pulse + 200
#define		minLogic1Pulse		averageLogic1Pulse - 200
#define		maxLogic1Pulse		averageLogic1Pulse + 200
#define		minStartPulse		averageStartPulse - 200
#define		maxStartPulse		averageStartPulse + 200

#define		PWR		0x01dec837

#define		ZER		0x01de00ff
#define		ONE		0x01de807f
#define		TWO		0x01de40bf
#define		THR		0x01dec03f
#define		FOU		0x01dea03f //need to decode 4-9
#define		FIV		0x01ded03f
#define		SIX		0x01dee03f
#define		SEV		0x01de503f
#define		EIG		0x01de403f
#define		NIN		0x01de303f

#define		FFW		0x01de18e7
#define		RRW		0x01de9867
#define		PLY		0x01de6897
#define		STP		0x01dee817

// DigitalSwitch.h

#ifndef _DIGITALSWITCH_h
#define _DIGITALSWITCH_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#include "Definitions.h"

class DigitalSwitch {
public:
	DigitalSwitch();
	
	//Callback Functions
	void S1On();
	void S1Off();
	void S2On();
	void S2Off();
	void S3On();
	void S3Off();
	void S4On();
	void S4Off();

	//UI Output
	String getStatusS1();
	String getStatusS2();
	String getStatusS3();
	String getStatusS4();
};

#endif


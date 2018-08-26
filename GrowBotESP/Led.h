// Led.h

#ifndef _LED_h
#define _LED_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class Led {
private:
	bool state;
	uint8_t pin;
public:
	Led(uint8_t pin, bool state);

	void switchState();
	void turnOn();
	void turnOff();
	void blink(uint8_t amount);
};

#endif


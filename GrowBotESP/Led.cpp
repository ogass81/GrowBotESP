// 
// 
// 

#include "Led.h"

Led::Led(uint8_t pin, bool state)
{
	this->pin = pin;
	this->state = state;
	pinMode(pin, OUTPUT);
}

void Led::switchState()
{
	if (state == true) {
		state = false;
		digitalWrite(pin , LOW);
	}
	else {
		state = true;
		digitalWrite(pin, HIGH);
	}
}

void Led::turnOn()
{
	if (state == false) {
		state = true;
		digitalWrite(pin, HIGH);
	}
}

void Led::turnOff()
{
	if (state == true) {
		state = false;
		digitalWrite(pin, LOW);
	}
}

void Led::blink(uint8_t amount)
{
	for (uint8_t i = 0; i < amount; i++) {
		switchState();
		delay(50);
	}
}

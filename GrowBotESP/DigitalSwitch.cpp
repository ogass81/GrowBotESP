// 
// 
// 

#include "DigitalSwitch.h"

DigitalSwitch::DigitalSwitch()
{
	pinMode(OUT_TOP_1, OUTPUT);
	pinMode(OUT_TOP_2, OUTPUT);
	pinMode(OUT_TOP_3, OUTPUT);
	pinMode(OUT_TOP_4, OUTPUT);

	//Set to off
	digitalWrite(OUT_TOP_1, HIGH);
	digitalWrite(OUT_TOP_2, HIGH);
	digitalWrite(OUT_TOP_3, HIGH);
	digitalWrite(OUT_TOP_4, HIGH);
}

void DigitalSwitch::S1On()
{
	if (digitalRead(OUT_TOP_1) == 1) {
		digitalWrite(OUT_TOP_1, LOW);
	}
}

void DigitalSwitch::S1Off()
{
	if (digitalRead(OUT_TOP_1) == 0) {
		digitalWrite(OUT_TOP_1, HIGH);
	}
}

void DigitalSwitch::S2On()
{
	if (digitalRead(OUT_TOP_2) == 1) {
		digitalWrite(OUT_TOP_2, LOW);
	}
}

void DigitalSwitch::S2Off()
{
	if (digitalRead(OUT_TOP_2) == 0) {
		digitalWrite(OUT_TOP_2, HIGH);
	}
}

void DigitalSwitch::S3On()
{
	if (digitalRead(OUT_TOP_3) == 1) {
		digitalWrite(OUT_TOP_3, LOW);
	}
}

void DigitalSwitch::S3Off()
{
	if (digitalRead(OUT_TOP_3) == 0) {
		digitalWrite(OUT_TOP_3, HIGH);
	}
}

void DigitalSwitch::S4On()
{
	if (digitalRead(OUT_TOP_4) == 1) {
		digitalWrite(OUT_TOP_4, LOW);
	}
}

void DigitalSwitch::S4Off()
{
	if (digitalRead(OUT_TOP_4) == 0) {
		digitalWrite(OUT_TOP_4, HIGH);
	}
}

String DigitalSwitch::getStatusS1()
{
	if (digitalRead(OUT_TOP_1) == 1) {
		return String("OFF");
	}
	return String("ON");
}

String DigitalSwitch::getStatusS2()
{
	if (digitalRead(OUT_TOP_2) == 1) {
		return String("OFF");
	}
	return String("ON");
}

String DigitalSwitch::getStatusS3()
{
	if (digitalRead(OUT_TOP_3) == 1) {
		return String("OFF");
	}
	return String("ON");
}

String DigitalSwitch::getStatusS4()
{
	if (digitalRead(OUT_TOP_4) == 1) {
		return String("OFF");
	}
	return String("ON");
}

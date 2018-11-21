// RCSocketController.h

#ifndef _RCSOCKETCONTROLLER_h
#define _RCSOCKETCONTROLLER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#include "Definitions.h"
#include <ArduinoJson.h>
#include <RCSwitch.h>
#include "RealTimeClock.h"
#include "LogEngine.h"
#include "Led.h"

extern LogEngine logengine;
extern RealTimeClock internalRTC;

class RCSocketCodeSet {
public:
	uint8_t id = 0;
	bool active = false;
	String title = "";
	
	int repeat = 0;

	unsigned long nReceivedValue[RC_SIGNALS];
	unsigned int nReceivedDelay[RC_SIGNALS];
	unsigned int nReceivedProtocol[RC_SIGNALS];
	unsigned int nReceivedBitlength[RC_SIGNALS];

	uint8_t signal_ptr = 0;

	RCSocketCodeSet(uint8_t id, int repeat);
	void incSignalPtr();
	void decSignalPtr();

	unsigned long getCurrentValue();
	unsigned int getCurrentDelay();
	unsigned int getCurrentBitlength();
	unsigned int getCurrentProtocol();

	unsigned long getValueFrom(uint8_t set);
	unsigned int getDelayFrom(uint8_t set);
	unsigned int getBitlengthFrom(uint8_t set);
	unsigned int getProtocolFrom(uint8_t set);

	void setCurrentValue(unsigned long value);
	void setCurrentDelay(unsigned int delay);
	void setCurrentBitlength(unsigned int bitlength);
	void setCurrentProtocol(unsigned int protocol);

	void switchCurrentProtocol();
	void switchSignalPtr();

	bool isNewSignal(long dec_val);
	uint8_t numberSignals();

	void serializeJSON(JsonObject & codeset, Scope scope);
	bool deserializeJSON(JsonObject & data);
};

class RCSocketController : public RCSwitch {
public:
	uint8_t receiver_pin;
	uint8_t transmitter_pin;
	uint8_t code_set_ptr = 0;

	RCSocketCodeSet *socketcode[RC_SOCKETS];
	bool learning = false;


	RCSocketController(uint8_t transmitter, uint8_t receiver);
	//Control Hardware
	void receiver_on();
	void receiver_off();
	void transmitter_on();
	void transmitter_off();

	void learningmode_on();
	void learningmode_on(int set);
	void learningmode_off();

	void learnPattern();
	void learnPattern(uint8_t code_set);
	void testSettings(uint8_t code_set);
	void resetSettings();
	void resetSettings(uint8_t set);

	void sendCode(int id);

	//Helper
	static const char * bin2tristate(const char * bin);
	static String dec2binWzerofill(unsigned long Dec, unsigned int bitLength);

	//UI
	String getTitle(int set);

	//Serialize
	void serializeJSON(JsonObject& data, Scope scope, uint8_t set = RC_SOCKETS);
	bool deserializeJSON(uint8_t set, JsonObject& data);
};

#endif

// 
// 
// 

#include "RCSocketController.h"
RCSocketCodeSet::RCSocketCodeSet(uint8_t id, int repeat)
{
	this->id = id;
	this->title = String("Signal " + String(id));
	this->active = false;
	this->repeat = repeat;

	for (uint8_t i = 0; i < RC_SIGNALS; i++) {
		nReceivedValue[i] = 0;
		nReceivedDelay[i] = 0;
		nReceivedBitlength[i] = 0;
		nReceivedProtocol[i] = 0;
	}
}

void RCSocketCodeSet::incSignalPtr()
{
	if (signal_ptr < RC_SIGNALS - 1) signal_ptr++;
	else signal_ptr = 0;
}

void RCSocketCodeSet::decSignalPtr()
{
	if (signal_ptr > 0 ) signal_ptr--;
	else signal_ptr = RC_SIGNALS - 1;
}

unsigned long RCSocketCodeSet::getCurrentValue()
{
	return nReceivedValue[signal_ptr];
}

unsigned int RCSocketCodeSet::getCurrentDelay()
{
	return nReceivedDelay[signal_ptr];
}

unsigned int RCSocketCodeSet::getCurrentBitlength()
{
	return nReceivedBitlength[signal_ptr];
}

unsigned int RCSocketCodeSet::getCurrentProtocol()
{
	return nReceivedProtocol[signal_ptr];
}

unsigned long RCSocketCodeSet::getValueFrom(uint8_t set)
{
	if (set < RC_SIGNALS && set >= 0) {
		return nReceivedValue[set];
	}
	else return 0;
}

unsigned int RCSocketCodeSet::getDelayFrom(uint8_t set)
{
	if (set < RC_SIGNALS && set >= 0) {
		return nReceivedDelay[set];
	}
	else return 0;
}

unsigned int RCSocketCodeSet::getBitlengthFrom(uint8_t set)
{
	if (set < RC_SIGNALS && set >= 0) {
		return nReceivedBitlength[set];
	}
	else return 0;
}

unsigned int RCSocketCodeSet::getProtocolFrom(uint8_t set)
{
	if (set < RC_SIGNALS && set >= 0) {
		return nReceivedProtocol[set];
	}
	else return 0;
}

void RCSocketCodeSet::setCurrentValue(unsigned long value)
{
	nReceivedValue[signal_ptr] = value;
}

void RCSocketCodeSet::setCurrentDelay(unsigned int delay)
{
	nReceivedDelay[signal_ptr] = delay;
}

void RCSocketCodeSet::setCurrentBitlength(unsigned int bitlength)
{
	nReceivedBitlength[signal_ptr] = bitlength;
}

void RCSocketCodeSet::setCurrentProtocol(unsigned int protocol)
{
	nReceivedProtocol[signal_ptr] = protocol;
}

void RCSocketCodeSet::switchCurrentProtocol()
{
	if (nReceivedValue[signal_ptr] != 0) {
		if (nReceivedProtocol[signal_ptr] < 6) nReceivedProtocol[signal_ptr]++;
		else nReceivedProtocol[signal_ptr] = 1;
	}
}

void RCSocketCodeSet::switchSignalPtr()
{
	if (signal_ptr < numberSignals() - 1) signal_ptr++;
	else signal_ptr = 0;
}

bool RCSocketCodeSet::isNewSignal(long dec_val)
{
	bool isnew = true;
	for (uint8_t i = 0; i < RC_SIGNALS; i++) {
		if (dec_val == nReceivedValue[i]) isnew = false;
	}
	return isnew;
}

uint8_t RCSocketCodeSet::numberSignals()
{
	for (uint8_t i = 0; i < RC_SIGNALS; i++) {
		if (nReceivedValue[i] == 0) {
			return i;
		}
	}
	return RC_SIGNALS;
}

void RCSocketCodeSet::serializeJSON(JsonObject &codeset, Scope scope)
{
	if (scope == LIST || scope == DETAILS) codeset["id"] = id;
	if (scope == LIST || scope == DETAILS) codeset["act"] = active;
	if (scope == LIST || scope == DETAILS) codeset["tit"] = title;
	
	if (scope == DETAILS) {
		codeset["obj"] = "RCSOCKET";
		codeset["rep"] = repeat;
	}
	
	if (scope == DETAILS) {
	JsonArray& values = codeset.createNestedArray("val");
	for (uint8_t j = 0; j < RC_SIGNALS; j++) values.add(nReceivedValue[j]);
	JsonArray& delays = codeset.createNestedArray("del");
	for (uint8_t j = 0; j < RC_SIGNALS; j++) delays.add(nReceivedDelay[j]);
	JsonArray& lengths = codeset.createNestedArray("len");
	for (uint8_t j = 0; j < RC_SIGNALS; j++) lengths.add(nReceivedBitlength[j]);
	JsonArray& protocols = codeset.createNestedArray("pro");
	for (uint8_t j = 0; j < RC_SIGNALS; j++) protocols.add(nReceivedProtocol[j]);
	}

	LOGDEBUG2(F("[Sensor]"), F("serializeJSON()"), F("OK: Sub-routine serialized codeset"), String(codeset.measureLength()), "", "");
}

bool RCSocketCodeSet::deserializeJSON(JsonObject &data)
{
	if (data.success() == true) {
		if (data["tit"] != "") title = data["tit"].asString();
		if (data["act"] != "") active = data["act"];
		if (data["rep"] != "") repeat = data["rep"];

		for (uint8_t j = 0; j < RC_SIGNALS; j++) if (data["value"][j] != "") nReceivedValue[j] = data["value"][j];
		for (uint8_t j = 0; j < RC_SIGNALS; j++) if (data["delay"][j] != "") nReceivedDelay[j] = data["delay"][j];
		for (uint8_t j = 0; j < RC_SIGNALS; j++) if (data["length"][j] != "") nReceivedBitlength[j] = data["length"][j];
		for (uint8_t j = 0; j < RC_SIGNALS; j++) if (data["proto"][j] != "") nReceivedProtocol[j] = data["proto"][j];
		LOGDEBUG2(F("[Sensor]"), F("deserializeJSON()"), F("OK: Sub-routine deserialized codeset"), String(data["id"].asString()), "", "");
	}
	else {
		LOGDEBUG2(F("[Sensor]"), F("deserializeJSON()"), F("ERROR: No Data for sub-routine to deserialize codeset"), F("Datasize"), String(data.size()), "");
	}
	return data.success();
}

RCSocketController::RCSocketController(uint8_t transmitter, uint8_t receiver)
{
	receiver_pin = receiver;
	transmitter_pin = transmitter;

	for (uint8_t i = 0; i < RC_SOCKETS; i++) {
		socketcode[i] = new RCSocketCodeSet(i, RC_REPEAT);
	}
	LOGMSG(F("[RCSocketController]"), F("OK: Started 433Mhz Controller."), F("Supported Sockets"), String(RC_SOCKETS), "");
}

void RCSocketController::receiver_on()
{
	enableReceive(receiver_pin);
	led[1]->turnOn();
	LOGDEBUG(F("[RCSocketController]"), F("receiver_on()"), F("OK: Turning on Receiver"), F("Pin"), String(receiver_pin), "");
}

void RCSocketController::receiver_off()
{
	disableReceive();
	led[1]->turnOff();
	LOGDEBUG(F("[RCSocketController]"), F("receiver_off()"), F("OK: Turning off Receiver"), F("Pin"), String(receiver_pin), "");
}

void RCSocketController::transmitter_on()
{
	enableTransmit(transmitter_pin);
	led[1]->turnOn();
	LOGDEBUG(F("[RCSocketController]"), F("transmitter_on()"), F("OK: Turning on Transmitter"), F("Pin"), String(transmitter_pin), "");
}

void RCSocketController::transmitter_off()
{
	disableTransmit();
	led[1]->turnOff();
	LOGDEBUG(F("[RCSocketController]"), F("transmitter_off()"), F("OK: Turning off Transmitter"), F("Pin"), String(transmitter_pin), "");
}

void RCSocketController::learningmode_on()
{
	LOGMSG(F("[RCSocketController]"), F("OK: Learning Mode set ON"), String(sensor_cycles), "@", String(RealTimeClock::printTime(sensor_cycles)));

	String keys[] = { "" };
	String values[] = { "" };
	logengine.addLogEntry(WARNING, "RCSocketController", "Halting System. Learning Mode On", keys, values, 0);

	receiver_on();
	learning = true;

	internalRTC.switch_haltstate();
}

void RCSocketController::learningmode_on(int set)
{
	if (set < RC_SOCKETS && learning == false) {
		code_set_ptr = set;
		learningmode_on();
	}
}

void RCSocketController::learningmode_off()
{
	if (learning == true) {
		receiver_off();
		learning = false;
		internalRTC.switch_haltstate();

		LOGMSG(F("[RCSocketController]"), F("OK: Learning Mode set OFF"), String(sensor_cycles), "@", String(RealTimeClock::printTime(sensor_cycles)));

		String keys[] = { "" };
		String values[] = { "" };
		logengine.addLogEntry(WARNING, "RCSocketController", "Learning Mode Off. Resuming System.", keys, values, 0);
	}
}

void RCSocketController::learnPattern()
{
	learnPattern(code_set_ptr);
}

void RCSocketController::learnPattern(uint8_t set)
{
	if (socketcode[set]->isNewSignal(getReceivedValue()) == true) {
		socketcode[set]->setCurrentValue(getReceivedValue());
		socketcode[set]->setCurrentBitlength(getReceivedBitlength());
		socketcode[set]->setCurrentDelay(getReceivedDelay());
		socketcode[set]->setCurrentProtocol(getReceivedProtocol());
		LOGDEBUG(F("[RCSocketController]"), F("learnPattern()"), F("OK: Received data -> New Signal"), String(socketcode[set]->getCurrentValue()), String(socketcode[set]->getCurrentDelay()), String(socketcode[set]->getCurrentProtocol()));
		socketcode[set]->incSignalPtr();
	}
	else {
		LOGDEBUG(F("[RCSocketController]"), F("learnPattern()"), F("OK: Received data -> Signal alread known "), String(socketcode[set]->getCurrentValue()), String(socketcode[set]->getCurrentDelay()), String(socketcode[set]->getCurrentProtocol()));
	}
}

void RCSocketController::testSettings()
{
	learningmode_off();

	socketcode[code_set_ptr]->active = true;

	for (uint8_t i = 0; i < 5; i++) {
		LOGDEBUG(F("[RCSocketController]"), F("testSettings()"), F("OK: Starting Test for Set"), String(code_set_ptr), F("Number of Signal"), String(socketcode[code_set_ptr]->numberSignals()));
		sendCode(code_set_ptr);
	}
	socketcode[code_set_ptr]->active = false;
}

void RCSocketController::resetSettings()
{
	learningmode_off();
	
	for (uint8_t i = 0; i < RC_SOCKETS; i++) {
		resetSettings(i);
	}
}

void RCSocketController::resetSettings(uint8_t set)
{
	learningmode_off();

	
	for (uint8_t i = 0; i < RC_SIGNALS; i++) {
		socketcode[set]->nReceivedDelay[i] = 0;
		socketcode[set]->nReceivedProtocol[i] = 0;
		socketcode[set]->nReceivedValue[i] = 0;
		socketcode[set]->nReceivedBitlength[i] = 0;
	}
	socketcode[set]->title = "Signal " + String(set);
	socketcode[set]->active = false;
	
}

void RCSocketController::sendCode(int set)
{
	LOGDEBUG(F("[RCSocketController]"), F("sendCode()"), F("OK: Called with"), F("Parameter"), String(set), String(learning));
	if (learning != true) {
		if (socketcode[set]->active == true) {
			transmitter_on();
			socketcode[set]->signal_ptr = 0;
			setRepeatTransmit(socketcode[set]->repeat);
			LOGDEBUG2(F("[RCSocketController]"), F("sendCode()"), F("OK: Set Signal Repeat"), F("Parameter"), String(socketcode[set]->repeat), "");

			for (uint8_t i = 0; i < socketcode[set]->numberSignals(); i++) {
				LOGDEBUG(F("[RCSocketController]"), F("sendCode()"), F("OK: Sending Signal"), String(socketcode[set]->getValueFrom(i)), String(socketcode[set]->getDelayFrom(i)), String(socketcode[set]->getProtocolFrom(i)));
				setPulseLength(socketcode[set]->getDelayFrom(i));
				setProtocol(socketcode[set]->getProtocolFrom(i));
				send(socketcode[set]->getValueFrom(i), socketcode[set]->getBitlengthFrom(i));
			}
			transmitter_off();
		}
		else {
			LOGDEBUG(F("[RCSocketController]"), F("sendCode()"), F("ERROR: Socket not active"), F("Parameter"), String(set), String(learning));
		}
	}
	else {
		LOGDEBUG(F("[RCSocketController]"), F("sendCode()"), F("ERROR: System in Learningmode"), F("Parameter"), String(set), String(learning));
	}
}

const char* RCSocketController::bin2tristate(const char* bin) {
	static char returnValue[50];
	int pos = 0;
	int pos2 = 0;
	while (bin[pos] != '\0' && bin[pos + 1] != '\0') {
		if (bin[pos] == '0' && bin[pos + 1] == '0') {
			returnValue[pos2] = '0';
		}
		else if (bin[pos] == '1' && bin[pos + 1] == '1') {
			returnValue[pos2] = '1';
		}
		else if (bin[pos] == '0' && bin[pos + 1] == '1') {
			returnValue[pos2] = 'F';
		}
		else {
			return "not applicable";
		}
		pos = pos + 2;
		pos2++;
	}
	returnValue[pos2] = '\0';
	return returnValue;
}

String RCSocketController::dec2binWzerofill(unsigned long Dec, unsigned int bitLength) {
	static char bin[64];
	unsigned int i = 0;

	while (Dec > 0) {
		bin[32 + i++] = ((Dec & 1) > 0) ? '1' : '0';
		Dec = Dec >> 1;
	}

	for (unsigned int j = 0; j< bitLength; j++) {
		if (j >= bitLength - i) {
			bin[j] = bin[31 + i - (j - (bitLength - i))];
		}
		else {
			bin[j] = '0';
		}
	}
	
	bin[bitLength] = '\0';
	

	return String(bin);
}

String RCSocketController::getTitle(int set)
{
	return String(socketcode[set]->title);
}

void RCSocketController::serializeJSON(uint8_t set, char * json, size_t maxSize, Scope scope)
{
	DynamicJsonBuffer jsonBuffer;

	JsonObject& socket = jsonBuffer.createObject();
	
	socketcode[set]->serializeJSON(socket, scope);
	
	socket.printTo(json, maxSize);
	LOGDEBUG2(F("[RCSocketController]"), F("serializeJSON()"), F("OK: Serialized remote sockets"), String(socket.measureLength()), String(maxSize), "");
}

void RCSocketController::serializeJSON(char * json, size_t maxSize, Scope scope)
{
	DynamicJsonBuffer jsonBuffer;

	jsonBuffer.clear();

	JsonObject& controller = jsonBuffer.createObject();
	
	controller["obj"] = "RCSOCKET";
	JsonArray& list = controller.createNestedArray("list");

	for (uint8_t i = 0; i < RC_SOCKETS; i++) {
		JsonObject& socket = jsonBuffer.createObject();
		socketcode[i]->serializeJSON(socket, scope);
		list.add(socket);
	}

	controller.printTo(json, maxSize);
	LOGDEBUG2(F("[RCSocketController]"), F("serializeJSON()"), F("OK: Serialized remote sockets"), String(controller.measureLength()), String(maxSize), "");
}

bool RCSocketController::deserializeJSON(uint8_t set, JsonObject & data)
{
	bool success = true;

	if (data.success() == true) {
		success = socketcode[set]->deserializeJSON(data);
		
		if (success == true) {
			LOGDEBUG2(F("[RCSocketController]"), F("deserializeJSON()"), F("OK: Deserialized remote socket"), String(set), F("Data size"), String(data.size()));
		}
		else {
			LOGDEBUG2(F("[RCSocketController]"), F("deserializeJSON()"), F("ERROR: Deserializing unsuccessful for socket"), String(set), F("Data size"), String(data.size()));
		}
	}
	else {
		LOGDEBUG2(F("[RCSocketController]"), F("deserializeJSON()"), F("ERROR: No Data to deserialize socket "), String(set), F("Data size"), String(data.size()));
	}

	return success;
}

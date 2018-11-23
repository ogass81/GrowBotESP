// 
// 
// 

#include "Trigger.h"


Trigger::Trigger() {
	reset();
}

bool Trigger::checkState()
{
	return false;
}

void Trigger::setState(int)
{
}

String Trigger::getTitle()
{

	return String(title);
}

String Trigger::getTitle(int parameter)
{
	return String(getTitle());
}

String Trigger::getSource()
{
	return String(source);
}

void Trigger::reset()
{
	
}

void Trigger::serializeJSON(JsonObject & data, Scope scope)
{
}

bool Trigger::deserializeJSON(JsonObject & data)
{
	return false;
}

void TimeTrigger::serializeJSON(JsonObject & data, Scope scope)
{
	if (scope == LIST || scope == DETAILS) {
		data["tit"] = title;
		data["act"] = active;
		data["src"] = source;
		data["typ"] = static_cast<int>(type);
		data["state"] = checkState();
	}

	if (scope == DETAILS) {
		data["obj"] = "TRIGGER";
		data["cat"] = cat;
		data["id"] = id;
		data["start_time"] = start_time;
		data["end_time"] = end_time;
		data["relop"] = static_cast<int>(relop);
		data["fire"] = fired;
		data["val"] = threshold;
		data["intv"] = static_cast<int>(interval);
		data["tol"] = tolerance;
	}

	LOGDEBUG2(F("[TimeTrigger]"), F("serializeJSON()"), F("OK: Serialized members for Trigger"), String(id), String(this->getTitle()), String(data.measureLength()));
}

bool TimeTrigger::deserializeJSON(JsonObject& data)
{
	if (data.success() == true) {
		if (data["tit"] != "") title = data["tit"].asString();
		if (data["act"] != "") active = data["act"];
		if (data["fire"] != "") fired = data["fire"];
		if (data["start_time"] != "") start_time = data["start_time"];
		if (data["end_time"] != "") end_time = data["end_time"];	
		if (data["val"] != "") threshold = data["val"];
		if (data["tol"] != "") tolerance = data["tol"];

		if (data["typ"] != "") {
			if (data["typ"] == 0) type = TIME;
			else if (data["typ"] == 1) type = SENSOR;
			else {
				relop = EQUAL;
				active = false;
			}
		}

		if (data["relop"] != "") {
			if (data["relop"] == 0) relop = SMALLER;
			else if (data["relop"] == 1) relop = EQUAL;
			else if (data["relop"] == 2) relop = GREATER;
			else if (data["relop"] == 3) relop = NOTEQUAL;
			else {
				relop = EQUAL;
				active = false;
			}
		}

		if (data["intv"] != "") {
			if (data["intv"] == 0) interval = REALTIME;
			else if (data["intv"] == 1) interval = TENSEC;
			else if (data["intv"] == 2) interval = TWENTYSEC;
			else if (data["intv"] == 3) interval = THIRTYSEC;
			else if (data["intv"] == 4) interval = ONEMIN;
			else if (data["intv"] == 5) interval = TWOMIN;
			else if (data["intv"] == 6) interval = FIVEMIN;
			else if (data["intv"] == 7) interval = QUARTER;
			else if (data["intv"] == 8) interval = HALF;
			else if (data["intv"] == 9) interval = ONE;
			else if (data["intv"] == 10) interval = TWO;
			else if (data["intv"] == 11) interval = THREE;
			else if (data["intv"] == 12) interval = FOUR;
			else if (data["intv"] == 13) interval = SIX;
			else if (data["intv"] == 14) interval = TWELVE;
			else if (data["intv"] == 15) interval = DAILY;
			else if (data["intv"] == 16) interval = BIDAILY;
			else if (data["intv"] == 17) interval = WEEKLY;
			else if (data["intv"] == 18) interval = BIWEEKLY;
			else {
				interval = ONEMIN;
				active = false;
			}
		}
		LOGDEBUG2(F("[TimeTrigger]"), F("deserializeJSON()"), F("OK: Deserialized members for Trigger"), String(id), String(this->getTitle()), String(this->source));
	}
	else {
		LOGDEBUG(F("[TimeTrigger]"), F("deserializeJSON()"), F("ERROR: No Data to deserialize members of Trigger"), String(id), String(this->getTitle()), String(data.size()));
	}
	return data.success();
}

void TimeTrigger::reset()
{
	title = "Timer " + String(id);

	active = false;
	fired = false;

	start_time = RealTimeClock::toEpochTime(internalRTC.defaulttime.Year, internalRTC.defaulttime.Month, internalRTC.defaulttime.Day, 0, 0, 0);
	end_time = RealTimeClock::toEpochTime(internalRTC.defaulttime.Year, internalRTC.defaulttime.Month, internalRTC.defaulttime.Day, 1, 0, 0);

	interval = ONEMIN;
	relop = EQUAL;
	threshold = 0;
	tolerance = 0;

	LOGDEBUG(F("[TimeTrigger]"), F("reset()"), F("OK: Reset"), String(id), String(this->getTitle()), String(this->source));
}


TimeTrigger::TimeTrigger(int id, uint8_t cat)
{
	this->id = id;
	this->cat = cat;
	this->type = TIME;
	this->source = String(F("RTC"));

	reset();

	LOGDEBUG3(F("[TimeTrigger]"), F("TimeTrigger()"), F("OK: Created new Time Trigger"), String(id), String(this->getTitle()), String(this->source));
}

bool TimeTrigger::checkState()
{
	long sensor_start = 0;
	long sensor_end = 0;
	
	bool state = false;
	
	//Transform Timestamp in Sensor Cycles as common base
	sensor_start = (start_time + internalRTC.timezone_offset) / SENS_FRQ_SEC;
	sensor_end = (end_time + internalRTC.timezone_offset) / SENS_FRQ_SEC;

	int remainder = 0;

	if (active == true) {
		if (sensor_cycles > sensor_start && sensor_cycles < sensor_end) {
			switch (interval) {
			case REALTIME:
				state = true;
				break;
			case TENSEC:
				remainder = checkStateInterval(sensor_start, 10);
				if (remainder == 0) {
					fired = true;
					state = true;
				}
				else if (remainder <=1) {
					if (fired != true) {
						state = true;
						fired = true;
					}
				}
				else fired = false;

				break;
			case TWENTYSEC:
				remainder = checkStateInterval(sensor_start, 20);
				if (remainder == 0) {
					fired = true;
					state = true;
				}
				else if (remainder <= 1) {
					if (fired != true) {
						state = true;
						fired = true;
					}
				}
				else fired = false;

				break;
			case THIRTYSEC:
				remainder = checkStateInterval(sensor_start, 30);
				if (remainder == 0) {
					fired = true;
					state = true;
				}
				else if (remainder <= 2) {
					if (fired != true) {
						state = true;
						fired = true;
					}
				}
				else fired = false;

				break;
			case ONEMIN:
				remainder = checkStateInterval(sensor_start, 60);
				if (remainder == 0) {
					fired = true;
					state = true;
				}
				else if (remainder <= 4) {
					if (fired != true) {
						state = true;
						fired = true;
					}
				}
				else fired = false;

				break;
			case TWOMIN:
				remainder = checkStateInterval(sensor_start, 120);
				if (remainder == 0) {
					fired = true;
					state = true;
				}
				else if (remainder <= 8) {
					if (fired != true) {
						state = true;
						fired = true;
					}
				}
				else fired = false;

				break;
			case FIVEMIN:
				remainder = checkStateInterval(sensor_start, 300);
				if (remainder == 0) {
					fired = true;
					state = true;
				}
				else if (remainder <= 20) {
					if (fired != true) {
						state = true;
						fired = true;
					}
				}
				else fired = false;

				break;
			case QUARTER:
				remainder = checkStateInterval(sensor_start, 900);
				if (remainder == 0) {
					fired = true;
					state = true;
				}
				else if (remainder <= 60) {
					if (fired != true) {
						state = true;
						fired = true;
					}
				}
				else fired = false;

				break;
			case HALF:
				remainder = checkStateInterval(sensor_start, 1800);
				if (remainder == 0) {
					fired = true;
					state = true;
				}
				else if (remainder <= 120) {
					if (fired != true) {
						state = true;
						fired = true;
					}
				}
				else fired = false;

				break;
			case ONE:
				remainder = checkStateInterval(sensor_start, 3600);
				if (remainder == 0) {
					fired = true;
					state = true;
				}
				else if (remainder <= 240) {
					if (fired != true) {
						state = true;
						fired = true;
					}
				}
				else fired = false;

				break;
			case TWO:
				remainder = checkStateInterval(sensor_start, 7200);
				if (remainder == 0) {
					fired = true;
					state = true;
				}
				else if (remainder <= 480) {
					if (fired != true) {
						state = true;
						fired = true;
					}
				}
				else fired = false;

				break;
			case THREE:
				remainder = checkStateInterval(sensor_start, 10800);
				if (remainder == 0) {
					fired = true;
					state = true;
				}
				else if (remainder <= 720) {
					if (fired != true) {
						state = true;
						fired = true;
					}
				}
				else fired = false;

				break;
			case FOUR:
				remainder = checkStateInterval(sensor_start, 14400);
				if (remainder == 0) {
					fired = true;
					state = true;
				}
				else if (remainder <= 960) {
					if (fired != true) {
						state = true;
						fired = true;
					}
				}
				else fired = false;

				break;
			case SIX:
				remainder = checkStateInterval(sensor_start, 21600);
				if (remainder == 0) {
					fired = true;
					state = true;
				}
				else if (remainder <= 1440) {
					if (fired != true) {
						state = true;
						fired = true;
					}
				}
				else fired = false;

				break;
			case TWELVE:
				remainder = checkStateInterval(sensor_start, 43200);
				if (remainder == 0) {
					fired = true;
					state = true;
				}
				else if (remainder <= 2880) {
					if (fired != true) {
						state = true;
						fired = true;
					}
				}
				else fired = false;

				break;
			case DAILY:
				remainder = checkStateInterval(sensor_start, 86400);
				if (remainder == 0) {
					fired = true;
					state = true;
				}
				else if (remainder <= 5760) {
					if (fired != true) {
						state = true;
						fired = true;
					}
				}
				else fired = false;

				break;
			case BIDAILY:
				remainder = checkStateInterval(sensor_start, 172800);
				if (remainder == 0) {
					fired = true;
					state = true;
				}
				else if (remainder <= 11520) {
					if (fired != true) {
						state = true;
						fired = true;
					}
				}
				else fired = false;

				break;
			case WEEKLY:
				remainder = checkStateInterval(sensor_start, 604800);
				if (remainder == 0) {
					fired = true;
					state = true;
				}
				else if (remainder <= 86400) {
					if (fired != true) {
						state = true;
						fired = true;
					}
				}
				else fired = false;

				break;
			case BIWEEKLY:
				remainder = checkStateInterval(sensor_start, 1209600);
				if (remainder == 0) {
					fired = true;
					state = true;
				}
				else if (remainder <= 86400) {
					if (fired != true) {
						state = true;
						fired = true;
					}
				}
				else fired = false;

				break;
			}
		}
	}
	LOGDEBUG3(F("[TimeTrigger]"), F("checkState()"), String("OK: Time Trigger Checked " + getTitle()), static_cast<int>(interval), fired, state);

	return state;
}

void TimeTrigger::setState(int)
{
}

int TimeTrigger::checkStateInterval(long sensor_start, long length)
{
	return 	((sensor_cycles - sensor_start) % (length / SENS_FRQ_SEC));
}


SensorTrigger::SensorTrigger(int id, uint8_t cat, SensorInterface *ptr)
{
	this->id = id;
	this->cat = cat;
	this->sens_ptr = ptr;
	this->type = SENSOR;
	this->source = sens_ptr->getTitle();

	reset();

	LOGDEBUG3(F("[SensorTrigger]"), F("SensorTrigger()"), F("OK: Created new Sensor Trigger"), String(id), String(this->getTitle()), String(this->source));
}

bool SensorTrigger::checkState()
{
	bool state = false;

	if (active == true) state = sens_ptr->compareWithValue(relop, interval, threshold, tolerance);
	else state = false;

	LOGDEBUG3(F("[SensorTrigger]"), F("checkState()"), String("OK: Sensor Trigger Checked " + getTitle()), threshold, static_cast<int>(interval), state);
	return state;
}

void SensorTrigger::setState(int)
{
}


void SensorTrigger::serializeJSON(JsonObject & data, Scope scope)
{
	if (scope == LIST || scope == DETAILS) {
		data["tit"] = title;
		data["act"] = active;
		data["src"] = source;
		data["typ"] = static_cast<int>(type);
		data["state"] = checkState();
	}

	if (scope == DETAILS) {
		data["obj"] = "TRIGGER";
		data["cat"] = cat;
		data["id"] = id;
		data["start_time"] = start_time;
		data["end_time"] = end_time;
		data["intv"] = static_cast<int>(interval);
		data["relop"] = static_cast<int>(relop);
		data["val"] = threshold;
		data["tol"] = tolerance;
	}

	LOGDEBUG2(F("[SensorTrigger]"), F("serializeJSON()"), F("OK: Serialized members for Trigger"), String(id), String(this->getTitle()), String(data.measureLength()));
}

bool SensorTrigger::deserializeJSON(JsonObject& data)
{
	if (data.success() == true) {
		if (data["tit"] != "") title = data["tit"].asString();
		if (data["act"] != "") active = data["act"];
		if (data["start_time"] != "") start_time = data["start_time"];
		if (data["end_time"] != "") end_time = data["end_time"];
		if (data["val"] != "") threshold = data["val"];
		if (data["tol"] != "") tolerance = data["tol"];

		if (data["relop"] != "") {
			if (data["relop"] == 0) relop = SMALLER;
			else if (data["relop"] == 1) relop = EQUAL;
			else if (data["relop"] == 2) relop = GREATER;
			else if (data["relop"] == 3) relop = NOTEQUAL;
			else {
				relop = EQUAL;
				active = false;
			}
		}

		if (data["intv"] != "") {
			if (data["intv"] == 0) interval = REALTIME;
			else if (data["intv"] == 1) interval = TENSEC;
			else if (data["intv"] == 2) interval = TWENTYSEC;
			else if (data["intv"] == 3) interval = THIRTYSEC;
			else if (data["intv"] == 4) interval = ONEMIN;
			else if (data["intv"] == 5) interval = TWOMIN;
			else if (data["intv"] == 6) interval = FIVEMIN;
			else if (data["intv"] == 7) interval = QUARTER;
			else if (data["intv"] == 8) interval = HALF;
			else if (data["intv"] == 9) interval = ONE;
			else if (data["intv"] == 10) interval = TWO;
			else if (data["intv"] == 11) interval = THREE;
			else if (data["intv"] == 12) interval = FOUR;
			else if (data["intv"] == 13) interval = SIX;
			else if (data["intv"] == 14) interval = TWELVE;
			else if (data["intv"] == 15) interval = DAILY;
			else if (data["intv"] == 16) interval = BIDAILY;
			else if (data["intv"] == 17) interval = WEEKLY;
			else if (data["intv"] == 18) interval = BIWEEKLY;
			else {
				interval = ONEMIN;
				active = false;
			}
		}
		LOGDEBUG2(F("[ SensorTrigger]"), F("deserializeJSON()"), F("OK: Deserialized members for Trigger"), String(id), String(this->getTitle()), String(this->source));
	}
	else {
		LOGDEBUG(F("[SensorTrigger]"), F("deserializeJSON()"), F("ERROR: No Data to deserialize members of Trigger"), String(id), String(this->getTitle()), String(data.size()));
	}
	return data.success();
}

void SensorTrigger::reset()
{
	title = "Comparator ";
	title += String(id);

	active = false;
	fired = false;
	
	start_time = RealTimeClock::toEpochTime(internalRTC.defaulttime.Year, internalRTC.defaulttime.Month, internalRTC.defaulttime.Day, 0, 0, 0);
	end_time = RealTimeClock::toEpochTime(internalRTC.defaulttime.Year, internalRTC.defaulttime.Month + 1, internalRTC.defaulttime.Day, 0, 0, 0);

	interval = ONEMIN;
	relop = EQUAL;
	threshold = 0;
	tolerance = 0;

	LOGDEBUG(F("[SensorTrigger]"), F("reset()"), F("OK: Reset"), String(id), String(this->getTitle()), String(this->source));
}

Counter::Counter(int id, uint8_t cat)
{
	this->id = id;
	this->cat = cat;
	this->type = COUNTER;

	reset();

	LOGDEBUG3(F("[Counter]"), F("Counter()"), F("OK: Created new Counter"), String(id), String(this->getTitle()), "");
}

bool Counter::checkState()
{
	bool state = false;
	if (active == true) {
		switch (relop) {
		case SMALLER:
			if (count <= threshold) state = true;
			else state = false;
			break;
		case GREATER:
			if (count > threshold) state = true;
			else state = false;
			break;
		case EQUAL:
			if (count == threshold) state = true;
			else state = false;
			break;
		case NOTEQUAL:
			if (count != threshold) state = true;
			else state = false;
			break;
		}
	}
	LOGDEBUG3(F("[Counter]"), F("checkState()"), String("OK: Counter Trigger Checked " + getTitle()), threshold, static_cast<int>(relop), state);

	return state;
}

void Counter::setState(int parameter)
{
	//count = count + static_cast<int>(reinterpret_cast<std::intptr_t>(parameter));
	if(parameter != 0) 	count = count + parameter;
	else count = 0;
}

void Counter::serializeJSON(JsonObject & data, Scope scope)
{
	if (scope == LIST || scope == DETAILS) {
		data["tit"] = title;
		data["act"] = active;
		data["src"] = source;
		data["typ"] = static_cast<int>(type);
		data["state"] = checkState();
		data["count"] = count;
	}

	if (scope == DETAILS) {
		data["obj"] = "TRIGGER";
		data["cat"] = cat;
		data["id"] = id;
		data["start_time"] = start_time;
		data["end_time"] = end_time;
		data["intv"] = static_cast<int>(interval);
		data["relop"] = static_cast<int>(relop);
		data["val"] = threshold;
		data["tol"] = tolerance;
	}

	LOGDEBUG2(F("[Counter]"), F("serializeJSON()"), F("OK: Serialized members for Trigger"), String(id), String(this->getTitle()), String(data.measureLength()));
}

bool Counter::deserializeJSON(JsonObject & data)
{
	if (data.success() == true) {
		if (data["tit"] != "") title = data["tit"].asString();
		if (data["act"] != "") active = data["act"];
		if (data["start_time"] != "") start_time = data["start_time"];
		if (data["end_time"] != "") end_time = data["end_time"];
		if (data["val"] != "") threshold = data["val"];
		if (data["count"] != "") count = data["count"];
		if (data["tol"] != "") tolerance = data["tol"];

		if (data["relop"] != "") {
			if (data["relop"] == 0) relop = SMALLER;
			else if (data["relop"] == 1) relop = EQUAL;
			else if (data["relop"] == 2) relop = GREATER;
			else if (data["relop"] == 3) relop = NOTEQUAL;
			else {
				relop = EQUAL;
				active = false;
			}
		}

		if (data["intv"] != "") {
			if (data["intv"] == 0) interval = REALTIME;
			else if (data["intv"] == 1) interval = TENSEC;
			else if (data["intv"] == 2) interval = TWENTYSEC;
			else if (data["intv"] == 3) interval = THIRTYSEC;
			else if (data["intv"] == 4) interval = ONEMIN;
			else if (data["intv"] == 5) interval = TWOMIN;
			else if (data["intv"] == 6) interval = FIVEMIN;
			else if (data["intv"] == 7) interval = QUARTER;
			else if (data["intv"] == 8) interval = HALF;
			else if (data["intv"] == 9) interval = ONE;
			else if (data["intv"] == 10) interval = TWO;
			else if (data["intv"] == 11) interval = THREE;
			else if (data["intv"] == 12) interval = FOUR;
			else if (data["intv"] == 13) interval = SIX;
			else if (data["intv"] == 14) interval = TWELVE;
			else if (data["intv"] == 15) interval = DAILY;
			else if (data["intv"] == 16) interval = BIDAILY;
			else if (data["intv"] == 17) interval = WEEKLY;
			else if (data["intv"] == 18) interval = BIWEEKLY;
			else {
				interval = ONEMIN;
				active = false;
			}
		}
		LOGDEBUG2(F("[Counter]"), F("deserializeJSON()"), F("OK: Deserialized members for Trigger"), String(id), String(this->getTitle()), String(this->source));
	}
	else {
		LOGDEBUG(F("[Counter]"), F("deserializeJSON()"), F("ERROR: No Data to deserialize members of Trigger"), String(id), String(this->getTitle()), String(data.size()));
	}
	return data.success();
}

void Counter::reset()
{
	title = "Counter ";
	title += String(id);

	source = "Internal Value";

	active = false;
	fired = false;

	start_time = RealTimeClock::toEpochTime(internalRTC.defaulttime.Year, internalRTC.defaulttime.Month, internalRTC.defaulttime.Day, 0, 0, 0);
	end_time = RealTimeClock::toEpochTime(internalRTC.defaulttime.Year, internalRTC.defaulttime.Month + 1, internalRTC.defaulttime.Day, 0, 0, 0);

	interval = ONEMIN;
	relop = EQUAL;
	count = 0;
	threshold = 0;
	tolerance = 0;

	LOGDEBUG(F("[Counter]"), F("reset()"), F("OK: Reset"), String(id), String(this->getTitle()), String(this->source));
}

Switch::Switch(int id, uint8_t cat)
	: Trigger()
{
	this->id = id;
	this->cat = cat;
	this->type = SWITCH;

	reset();

	LOGDEBUG3(F("[Counter]"), F("Counter()"), F("OK: Created new Switch"), String(id), String(this->getTitle()), "");
}

bool Switch::checkState()
{
	LOGDEBUG3(F("[Switch]"), F("checkState()"), String("OK: Switch Checked " + getTitle()), state, "" , "");

	return state;
}

void Switch::setState(int parameter)
{
	//int temp = static_cast<int>(reinterpret_cast<std::intptr_t>(parameter));

	if (parameter == 1) state = true;
	else state = false;
}

void Switch::serializeJSON(JsonObject & data, Scope scope)
{
	if (scope == LIST || scope == DETAILS) {
		data["tit"] = title;
		data["act"] = active;
		data["src"] = source;
		data["typ"] = static_cast<int>(type);
		data["state"] = state;
	}

	if (scope == DETAILS) {
		data["obj"] = "TRIGGER";
		data["cat"] = cat;
		data["id"] = id;
		data["start_time"] = start_time;
		data["end_time"] = end_time;
		data["intv"] = static_cast<int>(interval);
		data["relop"] = static_cast<int>(relop);
		data["val"] = threshold;
		data["tol"] = tolerance;
	}

	LOGDEBUG2(F("[Switch]"), F("serializeJSON()"), F("OK: Serialized members for Trigger"), String(id), String(this->getTitle()), String(data.measureLength()));
}

bool Switch::deserializeJSON(JsonObject & data)
{
	if (data.success() == true) {
		if (data["tit"] != "") title = data["tit"].asString();
		if (data["act"] != "") active = data["act"];
		if (data["start_time"] != "") start_time = data["start_time"];
		if (data["end_time"] != "") end_time = data["end_time"];
		if (data["val"] != "") threshold = data["val"];
		if (data["state"] != "") state = data["state"];
		if (data["tol"] != "") tolerance = data["tol"];

		if (data["relop"] != "") {
			if (data["relop"] == 0) relop = SMALLER;
			else if (data["relop"] == 1) relop = EQUAL;
			else if (data["relop"] == 2) relop = GREATER;
			else if (data["relop"] == 3) relop = NOTEQUAL;
			else {
				relop = EQUAL;
				active = false;
			}
		}

		if (data["intv"] != "") {
			if (data["intv"] == 0) interval = REALTIME;
			else if (data["intv"] == 1) interval = TENSEC;
			else if (data["intv"] == 2) interval = TWENTYSEC;
			else if (data["intv"] == 3) interval = THIRTYSEC;
			else if (data["intv"] == 4) interval = ONEMIN;
			else if (data["intv"] == 5) interval = TWOMIN;
			else if (data["intv"] == 6) interval = FIVEMIN;
			else if (data["intv"] == 7) interval = QUARTER;
			else if (data["intv"] == 8) interval = HALF;
			else if (data["intv"] == 9) interval = ONE;
			else if (data["intv"] == 10) interval = TWO;
			else if (data["intv"] == 11) interval = THREE;
			else if (data["intv"] == 12) interval = FOUR;
			else if (data["intv"] == 13) interval = SIX;
			else if (data["intv"] == 14) interval = TWELVE;
			else if (data["intv"] == 15) interval = DAILY;
			else if (data["intv"] == 16) interval = BIDAILY;
			else if (data["intv"] == 17) interval = WEEKLY;
			else if (data["intv"] == 18) interval = BIWEEKLY;
			else {
				interval = ONEMIN;
				active = false;
			}
		}
		LOGDEBUG2(F("[Switch]"), F("deserializeJSON()"), F("OK: Deserialized members for Trigger"), String(id), String(this->getTitle()), String(this->source));
	}
	else {
		LOGDEBUG(F("[Switch]"), F("deserializeJSON()"), F("ERROR: No Data to deserialize members of Trigger"), String(id), String(this->getTitle()), String(data.size()));
	}
	return data.success();
}

void Switch::reset()
{
	title = "Switch ";
	title += String(id);

	source = "Internal Value";

	active = false;
	fired = false;

	start_time = RealTimeClock::toEpochTime(internalRTC.defaulttime.Year, internalRTC.defaulttime.Month, internalRTC.defaulttime.Day, 0, 0, 0);
	end_time = RealTimeClock::toEpochTime(internalRTC.defaulttime.Year, internalRTC.defaulttime.Month + 1, internalRTC.defaulttime.Day, 0, 0, 0);

	interval = ONEMIN;
	relop = EQUAL;
	state = false;
	threshold = 0;
	tolerance = 0;

	LOGDEBUG(F("[Switch]"), F("reset()"), F("OK: Reset"), String(id), String(this->getTitle()), String(this->source));
}

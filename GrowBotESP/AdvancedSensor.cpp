// 
// 
// 

#include "AdvancedSensor.h"
String SensorInterface::getTitle()
{
	return String();
}

void SensorInterface::setUpperThreshold()
{
}

void SensorInterface::setLowerThreshold()
{
}

void SensorInterface::update()
{
}

bool SensorInterface::compareWithValue(RelOp relop, Interval interval, float value, int8_t tolerance)
{
	return false;
}

void SensorInterface::serializeJSON(JsonObject & data, Scope scope)
{
}

bool SensorInterface::deserializeJSON(JsonObject & data)
{
	return false;
}

void SensorInterface::reset()
{
}


template<class ReturnType>
String AdvancedSensor<ReturnType>::toNAN(ReturnType i)
{
	if (i == nan_val)	return String("#");
	else return String(i);
}

//Different Template Specification for Variable Types
template<class ReturnType>
ReturnType AdvancedSensor<ReturnType>::fromNAN(String str)
{
	if (str == "#") return nan_val;
	else return ReturnType(str.toInt());
}

template<>
float AdvancedSensor<float>::fromNAN(String str)
{
	if (str == "#") return nan_val;
	else return str.toFloat();
}

template<class ReturnType>
float AdvancedSensor<ReturnType>::average(int start, int num_elements, ReturnType * values, int max)
{
	LOGDEBUG2(F("[AdvancedSensor]"), F("average()"), String(title), String(start), String(num_elements), String(max));

	float dividend = 0;
	int divisor = 0;
	float avg = nan_val;

	int counter = 0;
	int element_ptr = start;

	while (counter <= num_elements)
	{
		//LOGDEBUG2(F("[AdvancedSensor]"), F("average()"), "Value at", String(element_ptr), String(counter), String(values[element_ptr]));
		if (values[element_ptr] != nan_val) {		
			dividend += (float)values[element_ptr];
			divisor++;
		}
		else break;

		//Raise counter
		counter++;
		//Move pointer to previous element
		element_ptr--;
		if (element_ptr < 0) {
			element_ptr = max - 1; //-1 because MAX is NaN value
		}
	}

	if (divisor != 0) {
		avg = dividend / divisor;
	}
	else {
		avg = nan_val;
		LOGDEBUG(F("[AdvancedSensor]"), F("average()"), F("Warning: Division by Zero. Returning NaN Value"), String(dividend), String(divisor), String(nan_val));
	}

	return float(avg);
}

template<class ReturnType>
AdvancedSensor<ReturnType>::AdvancedSensor(uint8_t id, String title, String unit, bool active, ReturnType nan_val, ReturnType min_val, ReturnType max_val) 
{
	this->id = id;
	this->title = title;
	this->unit = unit;
	this->active = active;
	this->nan_val = nan_val;
	this->min_val = min_val;
	this->max_val = max_val;

	reset();

	LOGDEBUG(F("[AdvancedSensor]"), F("AdvancedSensor()"), F("INFO:Free HEAP"), String(ESP.getFreeHeap()), "", "");
}

template<class ReturnType>
void AdvancedSensor<ReturnType>::reset()
{
	//this->lower_threshold = 0;
	//this->upper_threshold = 0;

	this->min_ptr = SENS_VALUES_MIN;
	this->hour_ptr = SENS_VALUES_HOUR;
	this->day_ptr = SENS_VALUES_DAY;
	this->month_ptr = SENS_VALUES_MONTH;

	for (int i = 0; i < SENS_VALUES_MIN; i++) this->min_values[i] = this->nan_val;
	for (int i = 0; i < SENS_VALUES_HOUR; i++) this->hour_values[i] = this->nan_val;
	for (int i = 0; i < SENS_VALUES_DAY; i++) this->day_values[i] = this->nan_val;
	for (int i = 0; i < SENS_VALUES_MONTH; i++) this->month_values[i] = this->nan_val;
}

template<class ReturnType>
String AdvancedSensor<ReturnType>::getTitle()
{
	return String(this->title);
}

template<class ReturnType>
ReturnType AdvancedSensor<ReturnType>::getValue(Interval interval)
{
	float avg;
	int int_length = 0;
	int element_count = 0;
	
	switch (interval) {
	case REALTIME:
		avg = min_values[min_ptr];
		break;
	case TENSEC:
		int_length = 60 / SENS_VALUES_MIN;
		element_count = 10 / int_length;

		avg = average(min_ptr, element_count, min_values, SENS_VALUES_MIN);
		break;
	case TWENTYSEC:
		int_length = 60 / SENS_VALUES_MIN;
		element_count = 20 / int_length;

		avg = average(min_ptr, element_count, min_values, SENS_VALUES_MIN);
		break;
	case THIRTYSEC:
		int_length = 60 / SENS_VALUES_MIN;
		element_count = 30 / int_length;

		avg = average(min_ptr, element_count, min_values, SENS_VALUES_MIN);
		break;
	case ONEMIN:
		int_length = 60 / SENS_VALUES_MIN;
		element_count = 60 / int_length;

		avg = average(min_ptr, element_count, min_values, SENS_VALUES_MIN);
		break;
	case TWOMIN:
		int_length = 3600 / SENS_VALUES_HOUR;
		element_count = 120 / int_length;

		avg = average(hour_ptr, element_count, hour_values, SENS_VALUES_HOUR);
		break;
	case FIVEMIN:
		int_length = 3600 / SENS_VALUES_HOUR;
		element_count = 300 / int_length;

		avg = average(hour_ptr, element_count, hour_values, SENS_VALUES_HOUR);
		break;
	case QUARTER:
		int_length = 3600 / SENS_VALUES_HOUR;
		element_count = 900 / int_length;

		avg = average(hour_ptr, element_count, hour_values, SENS_VALUES_HOUR);
		break;
	case HALF:
		int_length = 3600 / SENS_VALUES_HOUR;
		element_count = 1800 / int_length;

		avg = average(hour_ptr, element_count, hour_values, SENS_VALUES_HOUR);
		break;
	case ONE:
		int_length = 3600 / SENS_VALUES_HOUR;
		element_count = 3600 / int_length;

		avg = average(hour_ptr, element_count, hour_values, SENS_VALUES_HOUR);
		break;
	case TWO:
		int_length = 1440 / SENS_VALUES_DAY;
		element_count = 120 / int_length;

		avg = average(day_ptr, element_count, day_values, SENS_VALUES_DAY);
		break;
	case THREE:
		int_length = 1440 / SENS_VALUES_DAY;
		element_count = 180 / int_length;

		avg = average(day_ptr, element_count, day_values, SENS_VALUES_DAY);
		break;
	case FOUR:
		int_length = 1440 / SENS_VALUES_DAY;
		element_count = 240 / int_length;

		avg = average(day_ptr, element_count, day_values, SENS_VALUES_DAY);
		break;
	case SIX:
		int_length = 1440 / SENS_VALUES_DAY;
		element_count = 360 / int_length;

		avg = average(day_ptr, element_count, day_values, SENS_VALUES_DAY);
		break;
	case TWELVE:
		int_length = 1440 / SENS_VALUES_DAY;
		element_count = 720 / int_length;

		avg = average(day_ptr, element_count, day_values, SENS_VALUES_DAY);
		break;
	case DAILY:
		int_length = 1440 / SENS_VALUES_DAY;
		element_count = 1440 / int_length;

		avg = average(day_ptr, element_count, day_values, SENS_VALUES_DAY);
		break;
	case BIDAILY:
		int_length = 672 * NUM_MONTH / SENS_VALUES_MONTH;
		element_count = 48 / int_length;

		avg = average(month_ptr, element_count, month_values, SENS_VALUES_MONTH);
		break;
	case WEEKLY:
		int_length = 672 * NUM_MONTH / SENS_VALUES_MONTH;
		element_count = 168 / int_length;

		avg = average(month_ptr, element_count, month_values, SENS_VALUES_MONTH);
		break;
	case BIWEEKLY:
		int_length = 672 * NUM_MONTH / SENS_VALUES_MONTH;
		element_count = 336 / int_length;

		avg = average(month_ptr, element_count, month_values, SENS_VALUES_MONTH);
		break;
	}
	
	if ((ReturnType)avg == nan_val) return ReturnType(nan_val);
	else return ReturnType(round(avg));
}

template<class ReturnType>
void AdvancedSensor<ReturnType>::setUpperThreshold()
{
	this->upper_threshold = readRaw();
	LOGDEBUG(F("[AdvancedSensor]"), F("setUpperThreshold()"), F("Info: Set Upper Threshold to "), String(this->upper_threshold), "", "");
}

template<class ReturnType>
void AdvancedSensor<ReturnType>::setLowerThreshold()
{
	this->lower_threshold = readRaw();
	LOGDEBUG(F("[AdvancedSensor]"), F("setUpperThreshold()"), F("Info: Set Lower Threshold to "), String(this->lower_threshold), "", "");
}

template<>
float AdvancedSensor<float>::getValue(Interval interval)
{
	float avg;
	int int_length = 0;
	int element_count = 0;

	switch (interval) {
	case REALTIME:
		avg = min_values[min_ptr];
		break;
	case TENSEC:
		int_length = 60 / SENS_VALUES_MIN;
		element_count = 10 / int_length;

		avg = average(min_ptr, element_count, min_values, SENS_VALUES_MIN);
		break;
	case TWENTYSEC:
		int_length = 60 / SENS_VALUES_MIN;
		element_count = 20 / int_length;

		avg = average(min_ptr, element_count, min_values, SENS_VALUES_MIN);
		break;
	case THIRTYSEC:
		int_length = 60 / SENS_VALUES_MIN;
		element_count = 30 / int_length;

		avg = average(min_ptr, element_count, min_values, SENS_VALUES_MIN);
		break;
	case ONEMIN:
		int_length = 60 / SENS_VALUES_MIN;
		element_count = 60 / int_length;

		avg = average(min_ptr, element_count, min_values, SENS_VALUES_MIN);
		break;
	case TWOMIN:
		int_length = 3600 / SENS_VALUES_HOUR;
		element_count = 120 / int_length;

		avg = average(hour_ptr, element_count, hour_values, SENS_VALUES_HOUR);
		break;
	case FIVEMIN:
		int_length = 3600 / SENS_VALUES_HOUR;
		element_count = 300 / int_length;

		avg = average(hour_ptr, element_count, hour_values, SENS_VALUES_HOUR);
		break;
	case QUARTER:
		int_length = 3600 / SENS_VALUES_HOUR;
		element_count = 900 / int_length;

		avg = average(hour_ptr, element_count, hour_values, SENS_VALUES_HOUR);
		break;
	case HALF:
		int_length = 3600 / SENS_VALUES_HOUR;
		element_count = 1800 / int_length;

		avg = average(hour_ptr, element_count, hour_values, SENS_VALUES_HOUR);
		break;
	case ONE:
		int_length = 3600 / SENS_VALUES_HOUR;
		element_count = 3600 / int_length;

		avg = average(hour_ptr, element_count, hour_values, SENS_VALUES_HOUR);
		break;
	case TWO:
		int_length = 1440 / SENS_VALUES_DAY;
		element_count = 120 / int_length;

		avg = average(day_ptr, element_count, day_values, SENS_VALUES_DAY);
		break;
	case THREE:
		int_length = 1440 / SENS_VALUES_DAY;
		element_count = 180 / int_length;

		avg = average(day_ptr, element_count, day_values, SENS_VALUES_DAY);
		break;
	case FOUR:
		int_length = 1440 / SENS_VALUES_DAY;
		element_count = 240 / int_length;

		avg = average(day_ptr, element_count, day_values, SENS_VALUES_DAY);
		break;
	case SIX:
		int_length = 1440 / SENS_VALUES_DAY;
		element_count = 360 / int_length;

		avg = average(day_ptr, element_count, day_values, SENS_VALUES_DAY);
		break;
	case TWELVE:
		int_length = 1440 / SENS_VALUES_DAY;
		element_count = 720 / int_length;

		avg = average(day_ptr, element_count, day_values, SENS_VALUES_DAY);
		break;
	case DAILY:
		int_length = 1440 / SENS_VALUES_DAY;
		element_count = 1440 / int_length;

		avg = average(day_ptr, element_count, day_values, SENS_VALUES_DAY);
		break;
	case BIDAILY:
		int_length = 720 * NUM_MONTH / SENS_VALUES_MONTH;
		element_count = 48 / int_length;

		avg = average(month_ptr, element_count, month_values, SENS_VALUES_MONTH);
		break;
	case WEEKLY:
		int_length = 720 * NUM_MONTH / SENS_VALUES_MONTH;
		element_count = 168 / int_length;

		avg = average(month_ptr, element_count, month_values, SENS_VALUES_MONTH);
		break;
	case BIWEEKLY:
		int_length = 720 * NUM_MONTH / SENS_VALUES_MONTH;
		element_count = 336 / int_length;

		avg = average(month_ptr, element_count, month_values, SENS_VALUES_MONTH);
		break;
	}

	if (avg == nan_val) return nan_val;
	else return avg;
}

template<class ReturnType>
ReturnType AdvancedSensor<ReturnType>::readRaw()
{
	return ReturnType();
}

template<class ReturnType>
ReturnType AdvancedSensor<ReturnType>::readValue()
{
	return ReturnType();
}

template<class ReturnType>
void AdvancedSensor<ReturnType>::update()
{
	uint8_t interval = 0;
	uint8_t element_count = 0;

	//Minute -> read sensor every SENS_FRQ sec
	if (min_ptr < SENS_VALUES_MIN - 1) 	min_ptr++;
	else min_ptr = 0;

	min_values[min_ptr] = (ReturnType) round(readValue());

	//Hour -> calculate average every SENS_VALUES_HOUR * 24 / SENS_VALUES_DAY  -> Example: 1 Minute (12 entries in Hour array)
	if (sensor_cycles >= (SENS_VALUES_MIN * 60 / SENS_VALUES_HOUR) && sensor_cycles % (SENS_VALUES_MIN * 60  / SENS_VALUES_HOUR) == 0) {
		if (hour_ptr < SENS_VALUES_HOUR - 1) hour_ptr++;
		else hour_ptr = 0;

		element_count = SENS_VALUES_MIN * 60 / SENS_VALUES_HOUR;
		hour_values[hour_ptr] = (ReturnType)round(average(min_ptr, element_count, min_values, SENS_VALUES_MIN));
		LOGDEBUG(F("[AdvancedSensor]"), F("update()"), F("OK: Saved new Value in Hour Array"), String(getTitle()), String(hour_ptr), String(min_values[min_ptr]));
	}
	
	//Day -> calculate average every SENS_VALUES_HOUR * 24 / SENS_VALUES_DAY  -> Example: 1 Minute (12 entries in Hour array)
	if (sensor_cycles >= (SENS_VALUES_MIN * 60 * 24 / SENS_VALUES_DAY) && sensor_cycles % (SENS_VALUES_MIN * 60 * 24 / SENS_VALUES_DAY) == 0) {
		if (day_ptr < SENS_VALUES_DAY - 1) day_ptr++;
		else day_ptr = 0;

		element_count = SENS_VALUES_HOUR * 24 / SENS_VALUES_DAY;
		day_values[day_ptr] = (ReturnType) round(average(hour_ptr, element_count, hour_values, SENS_VALUES_HOUR));
		LOGDEBUG(F("[AdvancedSensor]"), F("update()"), F("OK: Saved new Value in Day Array"), String(getTitle()), String(day_ptr), String(day_values[day_ptr]));
	}

	//Month -> calculate average every SENS_VALUES_DAY * 28 / (SENS_VALUES_MONTH / NUM_MONTH)  -> Example: 1 Hour (60 entries in day array)
	if (sensor_cycles >= (SENS_VALUES_MIN * 60 * 24 * 28 * NUM_MONTH / SENS_VALUES_MONTH) && sensor_cycles % (SENS_VALUES_MIN * 60 * 24 * 28 * NUM_MONTH / SENS_VALUES_MONTH) == 0) {
		if (month_ptr < SENS_VALUES_MONTH - 1) 	month_ptr++;
		else month_ptr = 0;

		element_count = SENS_VALUES_DAY * 28 / (SENS_VALUES_MONTH / NUM_MONTH);
		month_values[month_ptr] = (ReturnType) round(average(day_ptr, element_count, day_values, SENS_VALUES_DAY));
		LOGDEBUG(F("[AdvancedSensor]"), F("update()"), F("OK: Saved new Value in Month Array"), String(getTitle()), String(month_ptr), String(month_values[month_ptr]));
	}
}

template<>
void AdvancedSensor<float>::update()
{
	uint8_t interval = 0;
	uint8_t element_count = 0;

	//Minute -> read sensor every SENS_FRQ sec
	if (min_ptr < SENS_VALUES_MIN - 1) 	min_ptr++;
	else min_ptr = 0;

	min_values[min_ptr] = readValue();

	//Hour -> calculate average every SENS_VALUES_HOUR * 24 / SENS_VALUES_DAY  -> Example: 1 Minute (12 entries in Hour array)
	if (sensor_cycles >= (SENS_VALUES_MIN * 60 / SENS_VALUES_HOUR) && sensor_cycles % (SENS_VALUES_MIN * 60 / SENS_VALUES_HOUR) == 0) {
		if (hour_ptr < SENS_VALUES_HOUR - 1) hour_ptr++;
		else hour_ptr = 0;

		element_count = SENS_VALUES_MIN * 60 / SENS_VALUES_HOUR;
		hour_values[hour_ptr] = average(min_ptr, element_count, min_values, SENS_VALUES_MIN);
		LOGDEBUG(F("[AdvancedSensor]"), F("update()"), F("OK: Saved new Value in Hour Array"), String(getTitle()), String(hour_ptr), String(min_values[min_ptr]));
	}

	//Day -> calculate average every SENS_VALUES_HOUR * 24 / SENS_VALUES_DAY  -> Example: 1 Minute (12 entries in Hour array)
	if (sensor_cycles >= (SENS_VALUES_MIN * 60 * 24 / SENS_VALUES_DAY) && sensor_cycles % (SENS_VALUES_MIN * 60 * 24 / SENS_VALUES_DAY) == 0) {
		if (day_ptr < SENS_VALUES_DAY - 1) day_ptr++;
		else day_ptr = 0;

		element_count = SENS_VALUES_HOUR * 24 / SENS_VALUES_DAY;
		day_values[day_ptr] = average(hour_ptr, element_count, hour_values, SENS_VALUES_HOUR);
		LOGDEBUG(F("[AdvancedSensor]"), F("update()"), F("OK: Saved new Value in Day Array"), String(getTitle()), String(day_ptr), String(day_values[day_ptr]));
	}

	//Month -> calculate average every SENS_VALUES_DAY * 28 / (SENS_VALUES_MONTH / NUM_MONTH)  -> Example: 1 Hour (60 entries in day array)
	if (sensor_cycles >= (SENS_VALUES_MIN * 60 * 24 * 28 * NUM_MONTH / SENS_VALUES_MONTH) && sensor_cycles % (SENS_VALUES_MIN * 60 * 24 * 28 * NUM_MONTH / SENS_VALUES_MONTH) == 0) {
		if (month_ptr < SENS_VALUES_MONTH - 1) 	month_ptr++;
		else month_ptr = 0;

		element_count = SENS_VALUES_DAY * 28 / (SENS_VALUES_MONTH / NUM_MONTH);
		month_values[month_ptr] = average(day_ptr, element_count, day_values, SENS_VALUES_DAY);
		LOGDEBUG(F("[AdvancedSensor]"), F("update()"), F("OK: Saved new Value in Month Array"), String(getTitle()), String(month_ptr), String(month_values[month_ptr]));
	}
}


template<class ReturnType>
bool AdvancedSensor<ReturnType>::compareWithValue(RelOp relop, Interval interval, float value, int8_t tolerance)
{
	bool state = false;
	ReturnType current_value = getValue(interval);
	float multi_low = (100.0f - (float)tolerance) / 100.0f;
	float multi_high = (100.0f + (float)tolerance) / 100.0f;

	ReturnType lower_boundery = (ReturnType)round(multi_low * value);
	ReturnType upper_boundery = (ReturnType)round(multi_high * value);

	LOGDEBUG2(F("[AdvancedSensor]"), F("compareWithValue()"), F("Info: Set Bounderies"), lower_boundery, upper_boundery, current_value);

	if (current_value != this->nan_val) {
		switch (relop) {
		case EQUAL:
			if (lower_boundery < upper_boundery && current_value >= lower_boundery && current_value <= upper_boundery) state = true;
			else if (current_value <= lower_boundery && current_value >= upper_boundery) state = true;
			else state = false;
			break;
		case NOTEQUAL:
			if (lower_boundery < upper_boundery && current_value < lower_boundery && current_value > upper_boundery) state = true;
			else if (current_value > lower_boundery && current_value < upper_boundery) state = true;
			else state = false;
			break;
		case SMALLER:
			if (current_value <= lower_boundery) state = true;
			else state = false;
			break;
		case GREATER:
			if (current_value > upper_boundery) state = true;
			else state = false;
			break;
		}
	}
	else state = false;

	return state;
}

template<>
bool AdvancedSensor<float>::compareWithValue(RelOp relop, Interval interval, float value, int8_t tolerance)
{
	bool state = false;
	float current_value = getValue(interval);

	float multi_low = (100.0f - (float)tolerance) / 100.0f;
	float multi_high = (100.0f + (float)tolerance) / 100.0f;

	float lower_boundery = multi_low * value;
	float upper_boundery = multi_high * value;

	LOGDEBUG2(F("[AdvancedSensor]"), F("compareWithValue()"), F("Info: Set Bounderies"), lower_boundery, upper_boundery, current_value);

	if (current_value != this->nan_val) {
		switch (relop) {
		case EQUAL:
			if (lower_boundery < upper_boundery && current_value >= lower_boundery && current_value <= upper_boundery) state = true;
			else if (current_value <= lower_boundery && current_value >= upper_boundery) state = true;
			else state = false;
			break;
		case NOTEQUAL:
			if (lower_boundery < upper_boundery && current_value < lower_boundery && current_value > upper_boundery) state = true;
			else if (current_value > lower_boundery && current_value < upper_boundery) state = true;
			else state = false;
			break;
		case SMALLER:
			if (current_value <= lower_boundery) state = true;
			else state = false;
			break;
		case GREATER:
			if (current_value > upper_boundery) state = true;
			else state = false;
			break;
		}
	}
	else state = false;

	return state;
}

template<class ReturnType>
void AdvancedSensor<ReturnType>::serializeJSON(JsonObject & data, Scope scope)
{
	//List View
	if (scope == LIST) {

		data["tit"] = title;
		data["unit"] = unit;
		data["typ"] = static_cast<int>(type);
		data["val"] = getValue(REALTIME);
	}

	//Sensor View
	//Header
	if (scope == HEADER || scope == DETAILS || scope == AVG || scope == DATE_MIN || scope == DATE_HOUR || scope == DATE_DAY || scope == DATE_MONTH || scope == DATE_ALL) {
		data["obj"] = "SENSOR";

	switch (scope) {
		case Scope::HEADER:
			data["scp"] = "HEADER";
			break;
		case Scope::DETAILS:
			data["scp"] = "DETAILS";
			break;
		case Scope::AVG:
			data["scp"] = "AVG";
			break;
		case Scope::DATE_MIN:
			data["scp"] = "MIN";
			break;
		case Scope::DATE_HOUR:
			data["scp"] = "HOUR";
			break;
		case Scope::DATE_DAY:
			data["scp"] = "DAY";
			break;
		case Scope::DATE_MONTH:
			data["scp"] = "MON";
			break;
		case Scope::DATE_ALL:
			data["scp"] = "ALL";
			break;
		}

		data["id"] = id;
		data["tit"] = title;
		data["typ"] = static_cast<int>(type);
		data["unit"] = unit;
		data["nan"] = nan_val;
		data["min"] = min_val;
		data["max"] = max_val;
		data["low"] = lower_threshold;
		data["high"] = upper_threshold;
	}

	//AVG Values
	if (scope == DETAILS || scope == AVG) {
		JsonObject& avg = data.createNestedObject("avg_vals");
		avg["last"] = getValue(REALTIME);
		avg["10s"] = getValue(TENSEC);
		avg["20s"] = getValue(TWENTYSEC);
		avg["30s"] = getValue(THIRTYSEC);
		avg["1min"] = getValue(ONEMIN);
		avg["2min"] = getValue(TWOMIN);
		avg["5min"] = getValue(FIVEMIN);
		avg["15min"] = getValue(QUARTER);
		avg["30min"] = getValue(HALF);
		avg["1h"] = getValue(ONE);
		avg["2h"] = getValue(TWO);
		avg["3h"] = getValue(THREE);
		avg["4h"] = getValue(FOUR);
		avg["6h"] = getValue(SIX);
		avg["12h"] = getValue(TWELVE);
		avg["1d"] = getValue(DAILY);
		avg["2d"] = getValue(BIDAILY);
		avg["1w"] = getValue(WEEKLY);
		avg["2w"] = getValue(BIWEEKLY);
	}
	//Hour Values
	if (scope == DETAILS || scope == DATE_MIN || scope == DATE_ALL) {
		data["min_ptr"] = min_ptr;
		data["frq"] = SENS_VALUES_MIN;
		JsonArray& minutes = data.createNestedArray("min_vals");
		for (int j = 0; j < SENS_VALUES_MIN; j++) minutes.add(toNAN(min_values[j]));
	}

	//Hour Values
	if (scope == DETAILS || scope == DATE_HOUR || scope == DATE_ALL) {
		data["h_ptr"] = hour_ptr;
		data["frq"] = SENS_VALUES_HOUR;
		JsonArray& hours = data.createNestedArray("h_vals");
		for (int j = 0; j < SENS_VALUES_HOUR; j++) hours.add(toNAN(hour_values[j]));
	}

	//Day Values
	if (scope == DETAILS || scope == DATE_DAY || scope == DATE_ALL) {
		data["d_ptr"] = day_ptr;
		data["frq"] = SENS_VALUES_DAY;
		JsonArray& days = data.createNestedArray("d_vals");
		for (int j = 0; j < SENS_VALUES_DAY; j++) days.add(toNAN(day_values[j]));
	}

	//Month Values
	if (scope == DETAILS || scope == DATE_MONTH || scope == DATE_ALL) {
		data["m_ptr"] = month_ptr;
		data["frq"] = SENS_VALUES_MONTH;
		JsonArray& month = data.createNestedArray("m_vals");
		for (int j = 0; j < SENS_VALUES_MONTH; j++) month.add(toNAN(month_values[j]));
	}

	LOGDEBUG2(F("[BaseSensor]"), F("serializeJSON()"), F("OK: Serialized members for Sensor"), String(data.measureLength()), "", "");
}

template<class ReturnType>
bool AdvancedSensor<ReturnType>::deserializeJSON(JsonObject & data)
{
	if (data.success() == true) {
		if (data["low"] != "") lower_threshold = data["low"];
		if (data["high"] != "") upper_threshold = data["high"];

		if (data["min_ptr"] != "") min_ptr = data["min_ptr"];
		if (data["h_ptr"] != "") hour_ptr = data["h_ptr"];
		if (data["d_ptr"] != "") day_ptr = data["d_ptr"];
		if (data["m_ptr"] != "") month_ptr = data["m_ptr"];

		for (int j = 0; j < SENS_VALUES_MIN; j++) if (data["min_vals"][j] != "") min_values[j] = fromNAN(data["min_vals"][j]);
		for (int j = 0; j < SENS_VALUES_HOUR; j++) if (data["h_vals"][j] != "") hour_values[j] = fromNAN(data["h_vals"][j]);
		for (int j = 0; j < SENS_VALUES_DAY; j++) if (data["d_vals"][j] != "") day_values[j] = fromNAN(data["d_vals"][j]);
		for (int j = 0; j < SENS_VALUES_MONTH; j++) if (data["m_vals"][j] != "") month_values[j] = fromNAN(data["m_vals"][j]);

		LOGDEBUG2(F("[BaseSensor]"), F("deserializeJSON()"), F("OK: Deserialized members for Sensor"), String(data["id"].asString()), "", "");
	}
	else {
		LOGDEBUG2(F("[BaseSensor]"), F("deserializeJSON()"), F("ERROR: No Data to deserialize members"), F("Datasize"), String(data.size()), "");
	}
	return data.success();
}

BMETemperature::BMETemperature(uint8_t id, BME280I2C * bme, bool active, String title, String unit, float nan_val, float min_val, float max_val) :
	AdvancedSensor<float>(id, title, unit, active, nan_val, min_val, max_val)
{
	this->bme = bme;

	this->type = TEMPERATURE;
}

float BMETemperature::readRaw()
{
	return float(bme->temp());
}

float BMETemperature::readValue()
{
	float adj_val = float(readRaw());

	LOGDEBUG3(F("[BMETemperature]"), F("readValue()"), F("Info: Temperature "), String(adj_val), "", "");

	return adj_val;
}

BMEHumidity::BMEHumidity(uint8_t id, BME280I2C * bme, bool active, String title, String unit, int8_t nan_val, int8_t min_val, int8_t max_val)
	:	AdvancedSensor<int8_t>(id, title, unit, active, nan_val, min_val, max_val)
{
	this->bme = bme;

	this->type = HUMIDITY;
}

int8_t BMEHumidity::readRaw()
{
	return float(bme->hum());
}

int8_t BMEHumidity::readValue()
{
	float adj_val = float(readRaw());

	LOGDEBUG3(F("[BMEHumidity]"), F("readValue()"), F("Info: Humidity "), String(adj_val), "", "");

	return float(adj_val);
}

BMEPressure::BMEPressure(uint8_t id, BME280I2C * bme, bool active, String title, String unit, float nan_val, float min_val, float max_val)
	: AdvancedSensor<float>(id, title, unit, active, nan_val, min_val, max_val)
{
	this->bme = bme;

	this->type = PRESSURE;
}

float BMEPressure::readRaw()
{
	return float(bme->pres());
}

float BMEPressure::readValue()
{
	float adj_val = float(readRaw() / 1000);

	LOGDEBUG3(F("[BMEPressure]"), F("readValue()"), F("Info: Air Pressure "), String(adj_val), "", "");
	return float(adj_val);
}

//DistanceLampSensor
DistanceLampSensor::DistanceLampSensor(uint8_t id, Ultrasonic *distance, bool active, String title, String unit, short nan_val, short min_val, short max_val)
	: AdvancedSensor<short>(id, title, unit, active, nan_val, min_val, max_val)
{
	this->distance = distance;

	this->type = DISTANCE;
}

short DistanceLampSensor::readRaw()
{
	float dividend = 0;
	int divisor = 0;
	float adj_val = 0;
	float current_value = 0;

	//Read 3 times just to make sure
	for (uint8_t i = 0; i < 3; i++) {
		delay(50);
		current_value = distance->distanceRead();
		LOGDEBUG3(F("[DistanceLampSensor]"), F("readRaw()"), F("Info: Distance to Bottom"), String(current_value), "", "");
		if (current_value != nan_val) {
			dividend += current_value;
			divisor++;
		}
	}

	if (divisor > 0) {
		adj_val = round(dividend / divisor);
		LOGDEBUG3(F("[DistanceLampSensor]"), F("readRaw()"), F("Info: Distance to Bottom"), String(adj_val), "", "");
	}
	else adj_val = nan_val;


	return (short) adj_val;
}

short DistanceLampSensor::readValue()
{
	return short(readRaw());
}

//Height
HeightSensor::HeightSensor(uint8_t id, Ultrasonic *distance1, Ultrasonic *distance2, bool active, String title, String unit, short nan_val, short min_val, short max_val)
	: AdvancedSensor<short>(id, title, unit, active, nan_val, min_val, max_val)
{
	this->distance1 = distance1;
	this->distance2 = distance2;

	this->type = HEIGHT;
}

float HeightSensor::readSensor(Ultrasonic *sensor)
{
	float dividend = 0;
	int divisor = 0;
	float adj_val = 0;
	float current_value = 0;

	//Read 3 times just to make sure
	for (uint8_t i = 0; i < 3; i++) {
		delay(50);
		current_value = sensor->distanceRead();
		if (current_value != nan_val) {
			dividend += current_value;
			divisor++;
		}
	}

	if (divisor > 0) {
		adj_val = round(dividend / divisor);
	}
	else adj_val = nan_val;

	return (short) adj_val;
}

short HeightSensor::readRaw()
{
	float toTop = readSensor(distance1);
	LOGDEBUG3(F("[HeightSensor]"), F("readRaw()"), F("Info: Distance to Roof"), String(toTop), "", "");

	float toBottom = readSensor(distance2);
	LOGDEBUG3(F("[HeightSensor]"), F("readRaw()"), F("Info: Distance to Bottom"), String(toBottom), "", "");

	if (upper_threshold != nan_val && lower_threshold != nan_val) return short(upper_threshold - toTop - toBottom - lower_threshold);
	else return (short) 0;
}

short HeightSensor::readValue()
{
	return short(readRaw());
}


CapacityMoistureSensor::CapacityMoistureSensor(uint8_t id, uint8_t pin, uint8_t resolution, uint8_t width, adc_attenuation_t attentuation, bool active, String title, String unit, short nan_val, short min_val, short max_val)
	: AdvancedSensor<short>(id, title, unit, active, nan_val, min_val, max_val)
{
	this->pin = pin;
	this->resolution = resolution;
	this->width = width;
	this->attentuation = attentuation;

	this->type = SOILMOISTURE;
	pinMode(pin, INPUT);
}

short CapacityMoistureSensor::readRaw()
{
	int dividend = 0;
	int divisor = 0;
	float adj_val;
	short current_value;

	//Set ADC resolution
	analogReadResolution(resolution);
	analogSetWidth(width);
	analogSetPinAttenuation(this->pin, attentuation);

	//Read 3 times just to make sure
	for (uint8_t i = 0; i < 3; i++) {
		delay(50);
		current_value = analogRead(this->pin);
		LOGDEBUG3(F("[CapacityMoistureSensor]"), F("readRaw()"), F("Info: Capacity Moisture Raw Value"), String(current_value), String(resolution), String(width));
		if (current_value != this->nan_val) {
			dividend += current_value;
			divisor++;
		}
	}

	if (divisor > 0) {
		adj_val = round(dividend / divisor);
		LOGDEBUG3(F("[CapacityMoistureSensor]"), F("readRaw()"), F("Info: Capacity Moisture Average Reading"), String(adj_val), "", "");
	}
	else adj_val = this->nan_val;

	return short(adj_val);
}

short CapacityMoistureSensor::readValue()
{
	short adj_val;

	adj_val = readRaw();

	//Caluclate Percentage if NaN not set
	if (adj_val != this->nan_val) {
		if (adj_val >= this->upper_threshold) {
			adj_val = short(0);
		}
		else if (adj_val <= this->lower_threshold) {
			adj_val = short(100);
		}
		else {
			adj_val = short(round(float(this->upper_threshold - adj_val) / float(this->upper_threshold - this->lower_threshold) * 100));
		}
	}
	LOGDEBUG3(F("[CapacityMoistureSensor]"), F("readValue()"), F("Info: Moisture Value % "), String(adj_val), String(this->lower_threshold), String(this->upper_threshold));
	return (short)adj_val;
}
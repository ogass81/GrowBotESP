// 
// 
// 

#include "RealTimeClock.h"

RealTimeClock::RealTimeClock(int year, int month, int day, int timezone_offset)
{
	source = "RTC";
	timezone_offset = 0;


	defaulttime.Year = year - 1970;
	defaulttime.Month = month;
	defaulttime.Day = day;
	defaulttime.Hour = 0;
	defaulttime.Minute = 0;
}

void RealTimeClock::updateTime(tmElements_t timeset, bool adjust)
{
	time_t timestamp = makeTime(timeset);
	
	if(adjust == true) timestamp += timezone_offset;
	
	breakTime(timestamp, timeset);
	
	/*
	//Update RTC with Values from Data Model
	setDate(timeset.Day, timeset.Month, (uint16_t)(timeset.Year + 1970));
	setTime(timeset.Hour, timeset.Minute, timeset.Second);
	*/

	LOGDEBUG2(F("[RealTimeClock]"), F("setTime(timeset)"), F("OK: Updated RTC"), String((uint16_t)(timeset.Year + 1970)), String(timeset.Month), String(timeset.Day));

	syncSensorCycles(timeset.Year, timeset.Month, timeset.Day, timeset.Hour, timeset.Minute, timeset.Second);
}

void RealTimeClock::updateTime(time_t timestamp, bool adjust)
{
	tmElements_t timeset; 

	if (adjust == true) timestamp += timezone_offset;

	breakTime(timestamp, timeset);
	
	/*
	//Update RTC with Values from Data Model
	setDate(timeset.Day, timeset.Month, (uint16_t)(timeset.Year + 1970));
	setTime(timeset.Hour, timeset.Minute, timeset.Second);
	*/

	LOGDEBUG2(F("[RealTimeClock]"), F("setTime(timestamp)"), F("OK: Updated RTC"), String((uint16_t)(timeset.Year + 1970)), String(timeset.Month), String(timeset.Day));

	syncSensorCycles(timeset.Year, timeset.Month, timeset.Day, timeset.Hour, timeset.Minute, timeset.Second);
}

void RealTimeClock::updateTime(uint8_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second, bool adjust)
{
	time_t timestamp = toEpochTime(year, month, day, hour, minute, second);
	if (adjust == true) timestamp += timezone_offset;

	tmElements_t timeset;

	breakTime(timestamp, timeset);

	/*
	//Update RTC with Values from Data Model
	setDate(timeset.Day, timeset.Month, (uint16_t)(timeset.Year + 1970));
	setTime(timeset.Hour, timeset.Minute, timeset.Second);
	*/

	LOGDEBUG2(F("[RealTimeClock]"), F("setTime(single values UINT8)"), F("OK: Updated RTC"), String((uint16_t)(timeset.Year + 1970)), String(timeset.Month), String(timeset.Day));

	syncSensorCycles(timeset.Year, timeset.Month, timeset.Day, timeset.Hour, timeset.Minute, timeset.Second);
}

void RealTimeClock::updateTime(int year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second, bool adjust)
{
	time_t timestamp = toEpochTime(year - 1970, month, day, hour, minute, second);
	if (adjust == true) timestamp += timezone_offset;

	tmElements_t timeset;
	
	breakTime(timestamp, timeset);

	/*
	//Update RTC with Values from Data Model
	setDate(timeset.Day, timeset.Month, (uint16_t)(timeset.Year + 1970));
	setTime(timeset.Hour, timeset.Minute, timeset.Second);
	*/
	
	LOGDEBUG2(F("[RealTimeClock]"), F("setTime(single values INT)"), F("OK: Updated RTC"), String((uint16_t)(timeset.Year + 1970)), String(timeset.Month), String(timeset.Day));

	syncSensorCycles(timeset.Year, timeset.Month, timeset.Day, timeset.Hour, timeset.Minute, timeset.Second);
}

void RealTimeClock::setDefaultTime()
{
	char month[12];
	int year, day, hour, minute, second;

	const char *monthName[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

	uint8_t monthIndex = 1;

	sscanf(__TIME__, "%d:%d:%d", &hour, &minute, &second);
	sscanf(__DATE__, "%s %d %d", month, &day, &year);

	for (monthIndex = 0; monthIndex < 12; monthIndex++) {
		if (strcmp(month, monthName[monthIndex]) == 0) break;
	}
	defaulttime.Day = day;
	defaulttime.Month = monthIndex + 1;
	defaulttime.Year = year - 1970;

	defaulttime.Hour = hour;
	defaulttime.Minute = minute;
	defaulttime.Second = second;
	
	/*
	//Update RTC with Values from Data Model
	setDate(timeset.Day, timeset.Month, (uint16_t)(timeset.Year + 1970));
	setTime(timeset.Hour, timeset.Minute, timeset.Second);
	*/

	LOGDEBUG2(F("[RealTimeClock]"), F("setdefaultTime()"), String(defaulttime.Year), String(defaulttime.Month), String(defaulttime.Day), String(__TIME__));

	syncSensorCycles(defaulttime.Year, defaulttime.Month, defaulttime.Day, defaulttime.Hour, defaulttime.Minute, defaulttime.Second);
}


time_t RealTimeClock::toEpochTime(uint8_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second)
{
	tmElements_t timeset;

	//Update Data Model with Values from input
	timeset.Year = year;
	timeset.Month = month;
	timeset.Day = day;

	timeset.Hour = hour;
	timeset.Minute = minute;
	timeset.Second = second;
	
	return makeTime(timeset);
}

time_t RealTimeClock::toEpochTime(int year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second)
{
	tmElements_t timeset;

	//Update Data Model with Values from input
	timeset.Year = year - 1970;
	timeset.Month = month;
	timeset.Day = day;

	timeset.Hour = hour;
	timeset.Minute = minute;
	timeset.Second = second;

	return makeTime(timeset);
}

/*
long RealTimeClock::getEpochTime()
{
	tmElements_t timeset;

	timeset.Year = getYear() - 1970;
	timeset.Month = getMonth();
	timeset.Day = getDay();

	timeset.Hour = getHours();
	timeset.Minute = getMinutes();
	timeset.Second = getSeconds();

	//LOGDEBUG2(F("[RealTimeClock]"), String(timeset.Year), String(timeset.Month), String(timeset.Day), String(timeset.Hour), String(timeset.Minute));
	
	return long(makeTime(timeset));
}
*/


String RealTimeClock::printDate(time_t timestamp)
{
	tmElements_t timeset;

	breakTime(timestamp, timeset);

	char dateStr[11];
	snprintf(dateStr, sizeof(dateStr), "%02d.%02d.%4d", timeset.Day, timeset.Month, timeset.Year);

	return String(dateStr);
}

String RealTimeClock::printTime(time_t timestamp)
{
	tmElements_t timeset;

	breakTime(timestamp, timeset);

	char timeStr[9];
	snprintf(timeStr, sizeof(timeStr), "%02d:%02d:%02d", timeset.Hour, timeset.Minute, timeset.Second);

	return String(timeStr);
}

/*
void RealTimeClock::syncSensorCycles()
{
	tmElements_t timeset;

	//Update Data Model with Values from RTC
	timeset.Year = getYear() - 1970;
	timeset.Month = getMonth();
	timeset.Day = getDay();

	timeset.Hour = getHours();
	timeset.Minute = getMinutes();
	timeset.Second = getSeconds();

	sensor_cycles =  makeTime(timeset) / SENS_FRQ_SEC;
	LOGDEBUG2(F("[RealTimeClock]"), F("syncSensorCycles()"), F("OK: Set new sensor cycle"), String(sensor_cycles), "", "");
}
*/


void RealTimeClock::switch_haltstate()
{
	if (haltstate == true) {
		sensor_cycles = sensor_cycles + ((millis() - haltstate_begin) / (1000 * SENS_FRQ_SEC));
		haltstate = false;
	}
	else {
		haltstate_begin = millis();
		haltstate = true;
	}
}

void RealTimeClock::syncSensorCycles(uint8_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second)
{
	sensor_cycles = toEpochTime(year, month, day, hour, minute, second) / SENS_FRQ_SEC;
	LOGDEBUG2(F("[RealTimeClock]"), F("syncSensorCycles(single values UINT8)"), F("OK: Set new sensor cycle"), String(sensor_cycles), "", "");
}

void RealTimeClock::syncSensorCycles(int year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second)
{
	sensor_cycles = toEpochTime(year - 1970, month, day, hour, minute, second) / SENS_FRQ_SEC;
	LOGDEBUG2(F("[RealTimeClock]"), F("syncSensorCycles(single values INT)"), F("OK: Set new sensor cycle"), String(sensor_cycles), "", "");
}

void RealTimeClock::syncSensorCycles(time_t timestamp)
{
	sensor_cycles = timestamp / SENS_FRQ_SEC;
	LOGDEBUG2(F("[RealTimeClock]"), F("syncSensorCycles(timestamp)"), F("OK: Set new sensor cycle"), String(sensor_cycles), "", "");
}

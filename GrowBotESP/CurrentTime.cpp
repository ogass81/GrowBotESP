// 
// 
// 

#include "CurrentTime.h"


CurrentTime::CurrentTime(int src)
	: RTCDue(src)
{
	source = "RTC";
}

long CurrentTime::epochTime()
{
	return epochTime(current_year, current_month, current_day, current_hour, current_minute, current_second);
}

long CurrentTime::epochTime(int year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second)
{
	long seconds = 0;
	
	//Save length
	year -= 2000;
		
	// seconds from 1970 till 1 jan 00:00:00 of the given year
	seconds = year * 365 * 24 * 60 * 60;
	for (int i = 0; i < year; i++) {
		if (LEAP_YEAR(i)) {
			seconds += 24 * 60 * 60;   // add extra days for leap years
		}
	}

	// add days for this year, months start from 1
	for (int i = 1; i < month; i++) {
		if ((i == 2) && LEAP_YEAR(year)) {
			seconds += 24 * 60 * 60 * 29;
		}
		else {
			seconds += 24 * 60 * 60* monthDays[i - 1];  //monthDay array starts from 0
		}
	}
	seconds += (day - 1) * 24 * 60 * 60;
	seconds += hour * 60 * 60;
	seconds += minute * 60;
	seconds += second;

	return long(seconds);
}

void CurrentTime::syncTimeObject() {
	if (user_update == true) {
		setSeconds(15);
		setMinutes(current_minute);
		setHours(current_hour);
		setDay(current_day);
		setMonth(current_month);
		setYear(current_year);
		user_update = false;
		LOGDEBUG(F("[CurrentTime]"), F("syncTimeObject()"), F("OK: Updating RTC from Time Object"), "", "", "");
		syncCycles();
	}
	else {
		current_second = getSeconds();
		current_minute = getMinutes();
		current_hour = getHours();
		current_day = getDay();
		current_month = getMonth();
		current_year = getYear();
		LOGDEBUG(F("[CurrentTime]"), F("syncTimeObject()"), F("OK: Updating Time Object from RTC"), "", "", "");
	}
}

void CurrentTime::syncCycles()
{	
	//Adjust Sensor Cycles to new date
	sensor_cycles = (CurrentTime::epochTime(current_year, current_month, current_day, current_hour, current_minute, 0)) / SENS_FRQ_SEC;
	LOGMSG(F("[CurrentTime]"), F("OK: Synced Cycles"), "New Cycle:", String(sensor_cycles), "");
}

void CurrentTime::updateRTC(int year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second) {
	setSeconds(15);
	setMinutes(minute);
	setHours(hour);
	setDay(day);
	setMonth(month);
	setYear(year);
	
	sensor_cycles = (CurrentTime::epochTime(current_year, current_month, current_day, current_hour, current_minute, 0)) / SENS_FRQ_SEC;
	LOGMSG(F("[CurrentTime]"), F("OK: Updating RTC from Parameters"), "New Cycle:", String(sensor_cycles), "");
}

void CurrentTime::updateRTCdefault() {
	char month[12];
	int year, day, hour, minute, second;

	const char *monthName[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

	uint8_t monthIndex = 1;


	sscanf(__TIME__, "%d:%d:%d", &hour, &minute, &second);
	sscanf(__DATE__, "%s %d %d", month, &day, &year);
	for (monthIndex = 0; monthIndex < 12; monthIndex++) {
		if (strcmp(month, monthName[monthIndex]) == 0) break;
	}

	setSeconds(15);
	setMinutes(minute);
	setHours(hour);
	setDay(day);
	setMonth(monthIndex+1);
	setYear(year);

	//Adjust Sensor Cycles to new date
	sensor_cycles = (CurrentTime::epochTime(current_year, current_month, current_day, current_hour, current_minute, 0)) / SENS_FRQ_SEC;
	LOGMSG(F("[CurrentTime]"), F("OK: Updating RTC from Compile Time"), "New Cycle:", String(sensor_cycles), "");
}

String CurrentTime::createDate() {
	char timeStr[11];
	snprintf(timeStr, sizeof(timeStr), "%02d.%02d.%4d", current_day, current_month, current_year);
	return timeStr;
}

String CurrentTime::createTime() {
	char timeStr[6];
	snprintf(timeStr, sizeof(timeStr), "%02d:%02d", current_hour, current_minute);
	return timeStr;
}

String CurrentTime::getTitle()
{
	return String(source);
}

void CurrentTime::incMinute() {
	if (current_minute < 59) {
		current_minute++;
	}
	else current_minute = 0;

	user_update = true;
}

void CurrentTime::decMinute() {
	if (current_minute > 0) {
		current_minute--;
	}
	else current_minute = 59;
	
	user_update = true;
}

void CurrentTime::incHour() {
	if (current_hour < 23) {
		current_hour++;
	}
	else current_hour = 0;

	user_update = true;
}

void CurrentTime::decHour() {
	if (current_hour > 0) {
		current_hour--;
	}
	else current_hour = 23;

	user_update = true;
}

void CurrentTime::incYear() {
	if (current_year < 2027) {
		current_year++;
	}
	else current_year = 2017;

	user_update = true;
}

void CurrentTime::decYear() {
	if (current_year > 2017) {
		current_year--;
	}
	else current_year = 2027;

	user_update = true;
}

void CurrentTime::incMonth() {
	if (current_month < 12) {
		current_month++;
	}
	else current_month = 1;

	user_update = true;
}

void CurrentTime::decMonth() {
	if (current_month > 1) {
		current_month--;
	}
	else current_month = 12;

	user_update = true;
}

void CurrentTime::incDay() {
	if (current_month == 1 || current_month == 3 || current_month == 5 || current_month == 7 || current_month == 8 || current_month == 10 || current_month == 12) {
		if (current_day < 31) {
			current_day++;
		}
		else current_day = 1;
	}
	else if (current_month == 4 || current_month == 6 || current_month == 9 || current_month == 11) {
		if (current_day < 30) {
			current_day++;
		}
		else current_day = 1;
	}
	else if (current_month == 2 || (current_year % 4) == 0) {
		if (current_day < 29) {
			current_day++;
		}
		else current_day = 1;
	}
	else {
		if (current_day < 28) {
			current_day++;
		}
		else current_day = 1;
	}
	user_update = true;
}

void CurrentTime::decDay() {
	if (current_month == 1 || current_month == 3 || current_month == 5 || current_month == 7 || current_month == 8 || current_month == 10 || current_month == 12) {
		if (current_day > 1) {
			current_day--;
		}
		else current_day = 31;
	}
	else if (current_month == 4 || current_month == 6 || current_month == 9 || current_month == 11) {
		if (current_day > 1) {
			current_day--;
		}
		else current_day = 30;
	}
	else if (current_month == 2 || (current_year % 4) == 0) {
		if (current_day > 1) {
			current_day--;
		}
		else current_day = 29;
	}
	else {
		if (current_day > 1) {
			current_day--;
		}
		else current_day = 28;
	}
	user_update = true;
}
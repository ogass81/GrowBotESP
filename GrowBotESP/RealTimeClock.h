// RealTimeClock.h

#ifndef _REALTIMECLOCK_h
#define _REALTIMECLOCK_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#include "Definitions.h"
#include <RTCDue.h>
#include <TimeLib.h>

extern long sensor_cycles;


class RealTimeClock : public RTCDue {

public:
	//Name
	String source;

	int timezone_offset;
	tmElements_t defaulttime;

	RealTimeClock(int src);

	void updateTime(tmElements_t timeset, bool adjust);
	void updateTime(time_t timestamp, bool adjust);
	void updateTime(uint8_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second, bool adjust);
	void updateTime(int year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second, bool adjust);

	void setDefaultTime();

	time_t static toEpochTime(uint8_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);
	time_t static toEpochTime(int year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);

	long getEpochTime();

	//UI Output
	String static printDate(time_t timestamp);
	String static printTime(time_t timestamp);

	void syncSensorCycles();
	void syncSensorCycles(uint8_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);
	void syncSensorCycles(int year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);
	void syncSensorCycles(time_t timestamp);
};
#endif


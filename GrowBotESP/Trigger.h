// Trigger.h

#ifndef _TRIGGER_h
#define _TRIGGER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#include "Definitions.h"
#include <ArduinoJson.h>
#include "RealTimeClock.h"
#include "Sensor.h"
#include "LogEngine.h"
extern LogEngine logengine;

//Globals
extern String debug;
extern RealTimeClock internalRTC;
extern Sensor *sensors[SENS_NUM];
extern long sensor_cycles;

class Trigger {
public:
	//Basic Information
	uint8_t id;
	String title;
	String source;
	bool active;
	bool fired;

	TriggerTypes type;

	//Time
	time_t start_time;
	time_t end_time;

	//Sensor and Thresholds
	//Threshold
	RelOp relop;
	short threshold;
	short tolerance; //Percent

	//Trigger repeat interval or trigger time windows for average
	Interval interval;

	//Constructor
	Trigger();

	//Check State
	virtual bool checkState();

	//UI Output
	String getTitle();
	String getSource();

	//Settings
	virtual void reset();

	//Serialization
	virtual void serializeJSON(uint8_t cat, uint8_t id, char* json, size_t maxSize, Scope scope);
	virtual bool deserializeJSON(JsonObject& data);
};

//Specialization of Trigger with predefined methods for RTC access
class TimeTrigger : public Trigger {
public:
	TimeTrigger(int id);
	bool checkState();

	void serializeJSON(uint8_t cat, uint8_t id, char* json, size_t maxSize, Scope scope);
	bool deserializeJSON(JsonObject& data);

	void reset();
private:
	int checkStateInterval(long sensor_start, uint8_t length);
};

//Specialization of Trigger with predefined methods for generic sensors
class SensorTrigger : public Trigger {
public:
	Sensor *sens_ptr;

	//Ref to Sensor Object
	SensorTrigger(int id, Sensor *ptr);

	bool checkState();

	void serializeJSON(uint8_t cat, uint8_t id, char* json, size_t maxSize, Scope scope);
	bool deserializeJSON(JsonObject& data);

	void reset();
};

//Trigger is a pair of sensor values and thresholds linked by a boolean operator -> can be TRUE or FALSE, repeatly checked
class TriggerCategory {
public:
	static void serializeJSON(Trigger *trigger[TRIGGER_TYPES][TRIGGER_SETS], char* json, size_t maxSize, Scope scope);
};
#endif


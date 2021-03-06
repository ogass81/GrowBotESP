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
#include "AdvancedSensor.h"
#include "LogEngine.h"
extern LogEngine logengine;

//Globals
extern String debug;
extern RealTimeClock internalRTC;
extern SensorInterface *sensors[SENS_NUM];
extern long sensor_cycles;

class Trigger {
public:
	//Basic Information
	int id;
	uint8_t cat;
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
	int threshold;
	short tolerance; //Percent

	//Trigger repeat interval or trigger time windows for average
	Interval interval;

	//Constructor
	Trigger();

	//Check State
	virtual bool checkState();
	virtual void setState(int);

	//UI Output
	String getTitle();
	String getTitle(int parameter);
	String getSource();

	//Settings
	virtual void reset();

	//Serialization
	virtual void serializeJSON(JsonObject& data, Scope scope);
	virtual bool deserializeJSON(JsonObject& data);
};

//Specialization of Trigger with predefined methods for RTC access
class TimeTrigger : public Trigger {
public:
	TimeTrigger(int id, uint8_t cat);
	bool checkState();
	void setState(int);

	void serializeJSON(JsonObject& data, Scope scope);
	bool deserializeJSON(JsonObject& data);

	void reset();
private:
	int checkStateInterval(long sensor_start, long length);
};

//Specialization of Trigger with predefined methods for generic sensors
class SensorTrigger : public Trigger {
public:
	SensorInterface *sens_ptr;

	//Ref to Sensor Object
	SensorTrigger(int id, uint8_t cat, SensorInterface *ptr);

	bool checkState();
	void setState(int);

	void serializeJSON(JsonObject& data, Scope scope);
	bool deserializeJSON(JsonObject& data);

	void reset();
};

//Specialization of Trigger with predefined methods for generic sensors
class Counter : public Trigger {
public:
	int count;

	//Ref to Sensor Object
	Counter(int id, uint8_t cat);

	bool checkState();
	void setState(int);

	void serializeJSON(JsonObject& data, Scope scope);
	bool deserializeJSON(JsonObject& data);

	void reset();
};

class Switch : public Trigger {
public:
	bool state;

	//Ref to Sensor Object
	Switch(int id, uint8_t cat);

	bool checkState();
	void setState(int);

	void serializeJSON(JsonObject& data, Scope scope);
	bool deserializeJSON(JsonObject& data);

	void reset();
};
#endif


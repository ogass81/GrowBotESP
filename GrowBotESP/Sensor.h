// Sensor.h

#ifndef _SENSOR_h
#define _SENSOR_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <DHT_U.h>
#include <DHT.h>
#include <ArduinoJson.h>
#include "Definitions.h"
#include "SerializationInterface.h"

#include "LogEngine.h"
extern LogEngine logengine;

extern DHT dht;
extern long sensor_cycles;

//Sensor Interface
class Sensor {
public:
	//UI Controls
	virtual int getAvgInt(Interval interval);
	virtual float getAvgFloat(Interval interval);

	virtual bool compareWithValue(RelOp relop, Interval interval, int value, int8_t tolerance);
	
	virtual String getTitle();
	virtual String getValue();
	
	virtual int getMaxValueInt(Scope scope);
	virtual int getMinValueInt(Scope scope);
	virtual int getElementValueInt(Scope scope, uint8_t element);

	virtual void setUpperThreshold();
	virtual void setLowerThreshold();

	virtual void update();

	//Settings
	virtual void reset();

	virtual void serializeJSON(uint8_t id, char* json, size_t maxSize, Scope scope);
	virtual bool deserializeJSON(JsonObject& data);
};


template <class ReturnType>
class BaseSensor : public Sensor {
private:
	//Methods to prepare NAN values for JSON serialization / deserialization
	String toNAN(ReturnType i);
	ReturnType fromNAN(String str);
public:
	//Sensor Info
	String title;
	String unit;
	SensorTypes type;

	ReturnType nan_val;
	ReturnType min_val;
	ReturnType max_val;
	ReturnType lower_threshold;
	ReturnType upper_threshold;

	//Sensor Hardware
	bool active;
	uint8_t pin;

	//Sensor Values
	ReturnType minute_values[SENS_VALUES_MIN];
	ReturnType hour_values[SENS_VALUES_HOUR]; 
	ReturnType day_values[SENS_VALUES_DAY];
	ReturnType month_values[SENS_VALUES_MONTH];
	ReturnType year_values[SENS_VALUES_YEAR];

	//Pointer to Values
	uint8_t minute_ptr = 255;
	uint8_t hour_ptr = 255;
	uint8_t day_ptr = 255;
	uint8_t month_ptr = 255;
	uint8_t year_ptr = 255;

	BaseSensor();

	//UI Controls
	int getAvgInt(Interval interval);
	float getAvgFloat(Interval interval);

	ReturnType getLastValue();
	ReturnType getTenSecAvg();
	ReturnType getTwentySecAvg();
	ReturnType getThirtySecAvg();
	ReturnType getOneMinAvg();
	ReturnType getTwoMinAvg();
	ReturnType getFiveMinAvg();
	ReturnType getQuarterHourAvg();
	ReturnType getHalfHourAvg();
	ReturnType getHourAvg();
	ReturnType getTwoHourAvg();
	ReturnType getThreeHourAvg();
	ReturnType getFourHourAvg();
	ReturnType getSixHourAvg();
	ReturnType getTwelveHourAvg();
	ReturnType getDayAvg();
	ReturnType getTwoDayAvg();
	ReturnType getWeekAvg();
	ReturnType getTwoWeekAvg();

	//UI Output
	virtual String getValue();
	String getTitle();
	int getMaxValueInt(Scope scope);
	int getMinValueInt(Scope scope);
	int getElementValueInt(Scope scope, uint8_t element);

	virtual void setUpperThreshold();
	virtual void setLowerThreshold();

	//Read new value and write to array
	virtual ReturnType readRaw();
	virtual ReturnType readValue();
	//Calculate Average
	ReturnType average(uint8_t start, uint8_t num_elements, ReturnType * values, uint8_t max);
	ReturnType minValue(ReturnType * values, uint8_t max);
	ReturnType maxValue(ReturnType * values, uint8_t max);

	void update();

	//Settings
	virtual void reset();
	
	//Serialize
	void serializeJSON(uint8_t id, char* json, size_t maxSize, Scope scope);
	bool deserializeJSON(JsonObject& data);
};

template <class ReturnType>
class AnalogMoistureSensor : public BaseSensor<ReturnType> {
public:
	uint8_t power_pin;

	AnalogMoistureSensor(uint8_t pin, uint8_t power_pin, bool active, String title, String unit, ReturnType nan_val, ReturnType min_val, ReturnType max_val, ReturnType lower_threshold, ReturnType upper_threshold);
	ReturnType readRaw();
	ReturnType readValue();
	String getValue();
	
	void setUpperThreshold();
	void setLowerThreshold();

	void reset();

	bool compareWithValue(RelOp relop, Interval interval, ReturnType value, int8_t tolerance);
};

class DHTTemperature : public BaseSensor<int8_t> {
private: 
	DHT *dht = NULL;
public:
	DHTTemperature(DHT *dht, bool active, String title, String unit, int8_t nan_val, int8_t min_val, int8_t max_val);
	int8_t readRaw();
	int8_t readValue();
	String getValue();

	void setUpperThreshold();
	void setLowerThreshold();

	void reset();

	bool compareWithValue(RelOp relop, Interval interval, int value, int8_t tolerance);
};

class DHTHumidity : public BaseSensor<int8_t> {
private:
	DHT *dht = NULL;
public:
	DHTHumidity(DHT *dht, bool active, String title, String unit, int8_t nan_val, int8_t min_val, int8_t max_val);
	int8_t readRaw();
	int8_t readValue();
	String getValue();

	void setUpperThreshold();
	void setLowerThreshold();

	void reset();

	bool compareWithValue(RelOp relop, Interval interval, int value, int8_t tolerance);
};
#endif
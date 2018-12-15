// AdvancedSensor.h

#ifndef _ADVANCEDSENSOR_h
#define _ADVANCEDSENSOR_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#include "Definitions.h"
#include <BME280I2C.h>
#include <Ultrasonic.h>
#include <ArduinoJson.h>
#include "LogEngine.h"

extern BME280I2C bme;
extern long sensor_cycles;
extern LogEngine logengine;


class SensorInterface {
public:
	virtual String getTitle();

	virtual void setUpperThreshold();
	virtual void setLowerThreshold();

	//Save new value
	virtual void update();

	//Compare with Trigger Threshold
	virtual bool compareWithValue(RelOp relop, Interval interval, float value, int8_t tolerance);

	//Serialize
	virtual void serializeJSON(JsonObject& data, Scope scope, Sort sort);
	virtual bool deserializeJSON(JsonObject& data);

	//Settings
	virtual void reset();
};


template <class ReturnType>
class AdvancedSensor : public SensorInterface {
private:
	//Methods to prepare NAN values for JSON serialization / deserialization
	String formatValueOut(ReturnType value);
	ReturnType formatValueIn(String str);

	//Calculate Average
	float average(int start, int num_elements, ReturnType * values, int max);
public:
	//Sensor Info
	uint8_t id;
	String title;
	String unit;
	SensorTypes type;
	uint8_t precision;
	bool active;

	//Sensor Value
	ReturnType nan_val;
	ReturnType min_val;
	ReturnType max_val;
	
	//Sensor Raw Value
	ReturnType lower_threshold;
	ReturnType upper_threshold;

	//Sensor Values
	ReturnType min_values[SENS_VALUES_MIN];
	ReturnType hour_values[SENS_VALUES_HOUR];
	ReturnType day_values[SENS_VALUES_DAY];
	ReturnType month_values[SENS_VALUES_MONTH];

	//Pointer to Values
	int min_ptr = SENS_VALUES_MIN;
	int hour_ptr = SENS_VALUES_HOUR;
	int day_ptr = SENS_VALUES_DAY;
	int month_ptr = SENS_VALUES_MONTH;

	AdvancedSensor(uint8_t id, String title, String unit, bool active, ReturnType nan_val, ReturnType min_val, ReturnType max_val, uint8_t precision = 0);
	void reset();

	String getTitle();
	ReturnType getValue(Interval interval);

	void setUpperThreshold();
	void setLowerThreshold();

	//Read new value
	virtual ReturnType readValue();
	//Read raw value
	virtual ReturnType readRaw();

	//Save new value
	void update();
	
	//Compare with Trigger Threshold
	bool compareWithValue(RelOp relop, Interval interval, float value, int8_t tolerance);

	//Serialize
	void serializeJSON(JsonObject& data, Scope scope, Sort sort);
	bool deserializeJSON(JsonObject& data);
};

class BMETemperature : public AdvancedSensor<float> {
private:
	BME280I2C *bme = NULL;
	uint8_t precision;

public:
	BMETemperature(uint8_t id, BME280I2C *bme, bool active, String title, String unit, float nan_val, float min_val, float max_val, uint8_t precision);
	float readRaw();
	float readValue();
};

class BMEHumidity : public AdvancedSensor<int8_t> {
private:
	BME280I2C *bme = NULL;
public:
	BMEHumidity(uint8_t id, BME280I2C *bme, bool active, String title, String unit, int8_t nan_val, int8_t min_val, int8_t max_val);
	int8_t readRaw();
	int8_t readValue();
};

class BMEPressure : public AdvancedSensor<float> {
private:
	BME280I2C *bme = NULL;

public:
	BMEPressure(uint8_t id, BME280I2C *bme, bool active, String title, String unit, float nan_val, float min_val, float max_val, uint8_t precision);
	float readRaw();
	float readValue();
};

class DistanceLampSensor : public AdvancedSensor<short> {
private:
	Ultrasonic *distance = NULL;
public:
	DistanceLampSensor(uint8_t id, Ultrasonic *distance, bool active, String title, String unit, short nan_val, short min_val, short max_val);
	short readRaw();
	short readValue();
};

class HeightSensor : public AdvancedSensor<short> {
private:
	Ultrasonic *distance1 = NULL;
	Ultrasonic *distance2 = NULL;
public:
	HeightSensor(uint8_t id, Ultrasonic *distance1, Ultrasonic *distance2, bool active, String title, String unit, short nan_val, short min_val, short max_val);

	float readSensor(Ultrasonic *sensor);

	short readRaw();
	short readValue();

	void serializeJSON(JsonObject& data, Scope scope, Sort sort);
};

class CapacityMoistureSensor : public AdvancedSensor<short> {
private:
	uint8_t resolution;
	uint8_t width;
	adc_attenuation_t attentuation;
	uint8_t pin;

public:
	CapacityMoistureSensor(uint8_t id, uint8_t pin, uint8_t resolution, uint8_t width, adc_attenuation_t attentuation, bool active, String title, String unit, short nan_val, short min_val, short max_val);
	short readRaw();
	short readValue();
	void serializeJSON(JsonObject& data, Scope scope, Sort sort);
};
#endif


// LogEngine.h

#ifndef _LOGENGINE_h
#define _LOGENGINE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
//Helper
#include "Definitions.h"
#include <ArduinoJson.h>

//Hardware
#include "SdFat.h"
#include "Led.h"
#include "RealTimeClock.h"

//Global Settings
extern long sensor_cycles;

//Hardware Globals
extern Led *led[3];
extern RealTimeClock internalRTC;

extern SdFat sd;

class LogEntry {
private:
	int id;
	long timestamp;
	LogTypes type;
	String origin;

	String message;

	uint8_t para_ptr;
	uint8_t para_size;
	String *keys;
	String *values;

public:
	LogEntry(int id, LogTypes type, String origin, String message, uint8_t para);
	~LogEntry();

	void addParameter(String key, String value);
	String serializeJSON();
};


class LogEngine {
private:
	uint8_t entry_ptr;
	LogEntry *log_buffer[LOGBUFFER_SIZE];

public:
	LogEngine();

	void begin();
	int counter;
	void addLogEntry(LogTypes type, String origin, String message, String keys[], String values[], uint8_t size);
	void serializeJSON(char* json, size_t maxSize, int end, int count);
	void reset();
private:
	void saveToFile();

	bool appendLinesToFile(const char * filename, String data[], uint8_t size);
	void readLinesFromFile(const char* filename, int counter, int start, int end, char * json, int size);
	int fileLength(const char* filename);
	bool resetFile(const char* filename);
};

#endif


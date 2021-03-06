// 
// 
// 

#include "LogEngine.h"

LogEntry::LogEntry(int id, LogTypes type, String origin, String message, uint8_t size)
{
	this->id = id;
	this->type = type;
	this->origin = origin;
	this->message = message;
	this->timestamp = sensor_cycles * SENS_FRQ_SEC - internalRTC.timezone_offset;

	this->para_size = size;
	this->para_ptr = 0;

	this->keys = new String[size];
	this->values = new String[size];

	LOGDEBUG3(F("LogEntry"), F("LogEntry()"), F("New LogEntry"), String(message), F("Parameters"), String(size));
}

LogEntry::~LogEntry()
{
	delete[] this->keys;
	delete[] this->values;
	LOGDEBUG3(F("LogEntry"), F("~LogEntry()"), F("Freed memory for Key->Value"), String(ESP.getFreeHeap()), "", "");
}

void LogEntry::addParameter(String key, String value)
{
	if (para_ptr < this->para_size) {
		this->keys[para_ptr] = key;
		this->values[para_ptr] = value;
		para_ptr++;
	}
}


String LogEntry::serializeJSON()
{
	StaticJsonBuffer<750> jsonBuffer;
	char json[1500];

	JsonObject& log = jsonBuffer.createObject();
	
	log["id"] = id;
	log["typ"] = static_cast<int>(type);
	log["time"] = timestamp;
	log["src"] = origin;
	log["msg"] = message;


	JsonArray& keys = log.createNestedArray("keys");
	for (uint8_t j = 0; j < this->para_size; j++) {
		if(this->keys[j] != "") keys.add(this->keys[j]);
	}

	JsonArray& values = log.createNestedArray("vals");
	for (uint8_t j = 0; j < this->para_size; j++) {
		if (this->values[j] != "") values.add(this->values[j]);
	}
	log.printTo(json, 1500);
	LOGDEBUG2(F("[LogEntry]"), F("serializeJSON()()"), F("OK: Serialized LogEntry"), String(id), String(static_cast<int>(type)), String(origin));

	return String(json);
}

void LogEntry::serializeJSON(JsonObject & data)
{
	data["id"] = id;
	data["typ"] = static_cast<int>(type);
	data["time"] = timestamp;
	data["src"] = origin;
	data["msg"] = message;


	JsonArray& keys = data.createNestedArray("keys");
	for (uint8_t j = 0; j < this->para_size; j++) {
		if (this->keys[j] != "") keys.add(this->keys[j]);
	}

	JsonArray& values = data.createNestedArray("vals");
	for (uint8_t j = 0; j < this->para_size; j++) {
		if (this->values[j] != "") values.add(this->values[j]);
	}
	LOGDEBUG2(F("[LogEntry]"), F("serializeJSON()(JsonObject)"), F("OK: Serialized LogEntry"), String(id), String(static_cast<int>(type)), String(origin));
}

LogEngine::LogEngine(const char* filename)
{
	this->counter = 0; // fileLength("log.json");
	this->filename = filename;
}

void LogEngine::begin()
{
	this->counter = fileLength();
}

void LogEngine::addLogEntry(LogTypes type, String origin, String message, String keys[], String values[], uint8_t size)
{
	if (counter == LOGBUFFER_FLUSH) {
		resetFile();
		this->counter = 0;
	}

	if (entry_ptr == LOGBUFFER_SIZE) {
		saveToFile();
	}

	log_buffer[entry_ptr] = new LogEntry(counter, type, origin, message, size);

	
	for (uint8_t i = 0; i < size; i++) {
		log_buffer[entry_ptr]->addParameter(keys[i], values[i]);
		//LOGDEBUG2(F("LogEngine"), F("addLogEntry()"), F("Adding Parameter"), String(keys[i]), String(values[i]), "");
	}
	LOGDEBUG3(F("LogEngine"), F("addLogEntry()"), F("OK: Add new LogEntry to memory queue"), String(counter), "Pointer", String(entry_ptr));

	this->entry_ptr++;
	this->counter++;

}

void LogEngine::serializeJSON(JsonObject & data, DynamicJsonBuffer& buffer, int end, int count)
{
	saveToFile();

	if (count == 0) count = LOGBUFFER_SIZE;
	
	data["num"] = counter;
	data["obj"] = "LOG";
	JsonArray& list = data.createNestedArray("list");
	
	int start = 0;
	int line_ptr = 0;

	if (end <= 0) {
		end = counter;
	}

	start = end - count;
	if (start < 0) start = 0;

	File file = SD.open(filename, FILE_READ);

	if (file) {

		while (file.available()) {
			String line = file.readStringUntil('\n');

			if (line_ptr < end) {
				if (line_ptr >= start) {
					JsonObject& element = buffer.parseObject(line);
					list.add(element);
				}
			}
			else break;
			line_ptr++;
		}
		file.close();
		LOGDEBUG2(F("LogEngine"), F("serializeJSON()"), F("OK: Loaded Log Entries from File"), String(start), String(end), String(list.size()));
	}
	else {
		LOGMSG(F("[LogEngine]"), F("ERROR: Could not read from file"), String(filename), "", "");
	}
}

void LogEngine::reset()
{
	saveToFile();
	resetFile();
	this->counter = 0;

	LOGDEBUG3(F("LogEngine"), F("reset()"), F("Reset Logengine"), String(entry_ptr), "", "")
}

void LogEngine::saveToFile()
{
	String output[LOGBUFFER_SIZE]; //MAX

	for (uint8_t i = 0; i < this->entry_ptr; i++) {
		if(log_buffer[i] != nullptr) output[i] = log_buffer[i]->serializeJSON();
	}
	appendLinesToFile(output, this->entry_ptr);


	//Free Memory
	for (uint8_t i = 0; i < this->entry_ptr; i++) if (log_buffer[i] != nullptr) delete this->log_buffer[i];
	this->entry_ptr = 0;

	LOGDEBUG2(F("LogEngine"), F("addLogEntry()"), F("Reset LogBuffer"), String(entry_ptr), "", "")
}

bool LogEngine::appendLinesToFile(String data[], uint8_t size)
{
	bool success = true;

	File file = SD.open(filename, FILE_APPEND);

	if (file) {
		//LOGDEBUG2(F("[LogEngine]"), F("saveSettings()"), F("File Open"), "", "", "");
		led[2]->turnOn();
		//Settings
		for (uint8_t i = 0; i < size; i++) {
			file.println(data[i]);
		}

		led[2]->turnOff();
		file.close();
		LOGDEBUG2(F("[LogEngine]"), F("appendLinesToFile()"), F("OK: Saved Log Entries to file"), String(filename), size, "");
	}
	else {
		LOGMSG(F("[LogEngine]"), F("ERROR: Could not write to file"), String(filename), "", "");
		success = false;
	}
	return success;
}

int LogEngine::fileLength()
{
	File file = SD.open(filename, FILE_READ);

	int counter = 0;

	if (file) {
		while (file.available()) {
			file.readStringUntil('\n');
			counter++;
		}
		file.close();
		LOGDEBUG1(F("LogEngine"), F("fileLength()"), F("OK: Determined file length"), String(filename), String(counter), "");
	}
	else {
		LOGMSG(F("[LogEngine]"), F("ERROR: Could not determine length of file"), String(filename), "", "");
	}

	return counter;
}

bool LogEngine::resetFile()
{
	if (SD.remove(filename)) {
		LOGDEBUG1(F("[LogEngine]"), F("resetFile()"), F("OK: Reset log file"), String(filename), "", "");
		return true;
	}
	else {
		LOGMSG(F("[LogEngine]"), F("ERROR: Could not reset log file"), String(filename), "", "");
		return false;
	}
}
// 
// 
// 

#include "Setting.h"

void Setting::reset()
{
	//Initialize Trigger
	for (int tcategory = 0; tcategory < TRIGGER_TYPES; tcategory++) {
		for (int tset = 0; tset < TRIGGER_SETS; tset++) {
			trigger[tcategory][tset]->reset();
		}
	}
	//Initialize Rulesets
	for (uint8_t k = 0; k < RULESETS_NUM; k++) {
		rulesets[k]->reset();
	}

	//Initialize Sensors
	for (uint8_t j = 0; j < SENS_NUM; j++) {
		sensors[j]->reset();
	}

	//Initialize Actionchains
	for (uint8_t j = 0; j < ACTIONCHAINS_NUM; j++) {
		actionchains[j]->reset();
	}

	//Initialize Actionchains
	for (uint8_t j = 0; j < ACTIONCHAINS_NUM; j++) {
		actionchains[j]->reset();
	}

	//Reset Sockets
	rcsocketcontroller->resetSettings();

	//Default Values
	wifi_ssid = "wgempire";
	wifi_pw = "ert456sdf233sa!!!";
	api_secret = "schnitzel";

	LOGMSG(F("[FileSystem]"), F("OK: Factory Reset to Default Settings (SSID | Password | API_secret)"), String(wifi_ssid), String(wifi_pw), String(api_secret));

	String keys[] = { "SSID", "Password", "API_secret" };
	String values[] = { wifi_ssid, wifi_pw, api_secret };
	logengine.addLogEntry(INFO, "Setting", "Factory Reset to Default Settings", keys, values, 3);
}

void Setting::serializeJSON(char * json, size_t maxSize)
{
	StaticJsonBuffer<JSONBUFFER_SIZE> jsonBuffer;

	JsonObject& settings = jsonBuffer.createObject();
	settings["obj"] = "SETTING";
	//Constants
	settings["firm_version"] = GROWBOT_FIRMWARE;
	settings["firm_date"] = __DATE__;
	settings["firm_time"] = __TIME__;
	settings["actions_num"] = ACTIONS_NUM;
	settings["actionschains_num"] = ACTIONCHAINS_NUM;
	settings["actionschains_length"] = ACTIONCHAIN_LENGTH;
	settings["rulesets_num"] = RULESETS_NUM;
	settings["trigger_sets"] = TRIGGER_SETS;
	settings["trigger_types"] = TRIGGER_TYPES;
	settings["sensor_num"] = SENS_NUM;
	settings["task_queue_length"] = TASK_QUEUE_LENGTH;
	settings["actionchain_task_maxduration"] = ACTIONCHAIN_TASK_MAXDURATION;
	settings["task_parallel_sec"] = TASK_PARALLEL_SEC;
	settings["rc_sockets_num"] = RC_SOCKETS;
	settings["rc_signals_num"] = RC_SIGNALS;
	settings["task_frq_sec"] = TASK_FRQ_SEC;
	settings["sens_frq_sec"] = SENS_FRQ_SEC;
	
	//Variable Settings	
	settings["wifi_SSID"] = wifi_ssid;
	settings["wifi_pw"] = wifi_pw;
	settings["api_secret"] = api_secret;
	//RTC problem 	settings["time"] = internalRTC.getEpochTime() - internalRTC.timezone_offset; //UTC
	settings["time"] = sensor_cycles * SENS_FRQ_SEC - internalRTC.timezone_offset;

	settings["timezone"] = internalRTC.timezone_offset;
	settings["log_size"] = logengine.counter;
	settings.printTo(json, maxSize);
	LOGDEBUG(F("[Setting]"), F("serializeJSON()"), F("OK: Serialized Overall Settings"), String(settings.measureLength()), String(maxSize), "");
}

bool Setting::deserializeJSON(JsonObject & data)
{

	long stored_time = 0, rtc_time = 0;

	if (data.success() == true) {
		if (data["wifi_SSID"].asString() != "") {
			wifi_ssid = data["wifi_SSID"].asString();
			LOGMSG(F("[Setting]"), F("OK: Wifi SSID loaded"), String(wifi_ssid), "", "");
		}
		else {
			LOGMSG(F("[Setting]"), F("ERROR: No Wifi SSID loaded"), "", "", "");
		}

		if (data["wifi_pw"].asString() != "") {
			wifi_pw = data["wifi_pw"].asString();
			LOGMSG(F("[Setting]"), F("OK: Wifi SSID loaded"), String(wifi_pw.substring(0, 3)) + "*****", "", "");
		}
		else {
			LOGMSG(F("[Setting]"), F("ERROR: No Wifi password loaded"), "", "", "");
		}

		if (data["api_secret"].asString() != "") {
			api_secret = data["api_secret"].asString();
			LOGMSG(F("[Setting]"), F("OK: Loaded API secret"), String(api_secret.substring(0, 3)) + "*****", "", "");
		}
		else {
			LOGMSG(F("[Setting]"), F("WARNING: No API secret loaded"), "", "", "");
		}

		if (data["timezone"] != "") {
			//Set RTC
			internalRTC.timezone_offset = data["timezone"];

			LOGMSG(F("[Setting]"), F("OK: Updated Timezone from Settings"), String(internalRTC.timezone_offset), "", "");
		}

		else {
			LOGMSG(F("[Setting]"), F("WARNING: No Timestamp loaded"), "", "", "");
		}

		if (data["time"] != "") {
			//Set RTC
			time_t timestamp = data["time"];
			internalRTC.updateTime(timestamp, true);

			LOGMSG(F("[Setting]"), F("OK: Updated Time from Settings"), String(timestamp), String(sensor_cycles), "");
		}
		else {
			LOGMSG(F("[Setting]"), F("WARNING: No Timezone Offset loaded"), "", "", "");
		}
	}
	else {
		LOGDEBUG(F("[Setting]"), F("deserializeJSON()"), F("ERROR: No Data to deserialize settings"), F("Datasize"), String(data.size()), "");
	}
	return data.success();
}


bool Setting::saveSettings(const char* filename)
{
	File file;

	char json[JSONCHAR_SIZE];
	int bytes;
	bool complete;
	bool success = true;

	if (file.open(filename, O_CREAT | O_TRUNC | O_WRITE)) {
		//LOGDEBUG2(F("[FileSystem]"), F("saveSettings()"), F("File Open"), "", "", "");
		led[2]->turnOn();
		//Settings
		Setting::serializeJSON(json, JSONCHAR_SIZE);
		file.println(json);
		LOGDEBUG2(F("[FileSystem]"), F("saveSettings()"), F("OK: Saved Overall Settings"), "", "", "");

		for (uint8_t i = 0; i < TRIGGER_TYPES; i++) {
			for (uint8_t j = 0; j < TRIGGER_SETS; j++) {
				trigger[i][j]->serializeJSON(i, j, json, JSONCHAR_SIZE, DETAILS);
				led[2]->switchState();
				file.println(json);
				//LOGDEBUG2(F("[FileSystem]"), F("saveSettings()"), F("OK: Saved Trigger"), F("Cat | Id"), String(i), String(j));
			}
		}
		for (uint8_t i = 0; i < RULESETS_NUM; i++) {
			rulesets[i]->serializeJSON(i, json, JSONCHAR_SIZE, DETAILS);
			led[2]->switchState();
			file.println(json);
			//LOGDEBUG2(F("[FileSystem]"), F("saveSettings()"), F("OK: Saved Rule"), F("Id"), String(i), "");
		}
		for (uint8_t i = 0; i < ACTIONCHAINS_NUM; i++) {
			actionchains[i]->serializeJSON(i, json, JSONCHAR_SIZE, DETAILS);
			led[2]->switchState();
			file.println(json);
			//LOGDEBUG2(F("[FileSystem]"), F("saveSettings()"), F("OK: Saved Actionschain"), F("Id"), String(i), "");
		}
		for (uint8_t i = 0; i < SENS_NUM; i++) {
			sensors[i]->serializeJSON(i, json, JSONCHAR_SIZE, DETAILS);
			led[2]->switchState();
			file.println(json);
			//LOGDEBUG2(F("[FileSystem]"), F("saveSettings()"), F("OK: Saved Sensor"), F("Id"), String(i), "");
		}
		for (uint8_t i = 0; i < RC_SOCKETS; i++) {
			rcsocketcontroller->serializeJSON(i, json, JSONCHAR_SIZE, DETAILS);
			led[2]->switchState();
			file.println(json);
			//LOGDEBUG2(F("[FileSystem]"), F("saveSettings()"), F("OK: Saved Remote Socket"), F("Id"), String(i), "");
		}
		led[2]->turnOff();
		file.close();

		LOGMSG(F("[FileSystem]"), F("OK: Saved Settings to file:"), String(filename), "", "");
	}
	else {
		LOGMSG(F("[FileSystem]"), F("ERROR: Could not write to file:"), String(filename), "", "");
	}
	return success;
}

bool Setting::loadSettings(const char* filename)
{
	File file;

	String json;
	int cat = 0;
	int id = 0;
	int j = 0;
	bool success = true;

	if (file.open(filename, O_READ)) {

		while (file.available()) {
			//Buffer Needs to be here ...
			DynamicJsonBuffer jsonBuffer;

			json = file.readStringUntil('\n');

			JsonObject& node = jsonBuffer.parseObject(json);

			cat = 0;
			id = 0;

			if (node.success()) {
				if (node["obj"] == "SETTING") {
					success = Setting::deserializeJSON(node);
				}
				else if (node["obj"] == "SENSOR") {
					id = (int)node["id"];
					if (id < SENS_NUM) {
						success = sensors[id]->deserializeJSON(node);
						LOGDEBUG(F("[FileSystem]"), F("loadSettings()"), F("OK: Loaded Sensor"), F("Id"), String(id), "");
					}
					else {
						LOGDEBUG(F("[FileSystem]"), F("loadSettings()"), F("ERROR: Invalid Sensor"), F("Id"), String(id), "");
						success = false;
					}
				}
				else if (node["obj"] == "TRIGGER") {
					id = (int)node["id"];
					cat = (int)node["cat"];

					if (id <= TRIGGER_SETS && cat <= TRIGGER_TYPES) {
						success = trigger[cat][id]->deserializeJSON(node);
						LOGDEBUG(F("[FileSystem]"), F("loadSettings()"), F("OK: Loaded Trigger"), F("Cat | Id"), String(id), String(cat));
					}
					else {
						LOGDEBUG(F("[FileSystem]"), F("loadSettings()"), F("ERROR: Invalid Trigger"), F("Cat | Id"), String(cat), String(id));
						success = false;
					}
				}
				else if (node["obj"] == "RULESET") {
					id = (int)node["id"];
					if (id < RULESETS_NUM) {
						success = rulesets[id]->deserializeJSON(node);
						LOGDEBUG(F("[FileSystem]"), F("loadSettings()"), F("OK: Loaded Ruleset"), F("Id"), String(id), "");
					}
					else {
						LOGDEBUG(F("[FileSystem]"), F("loadSettings()"), F("ERROR: Invalid Ruleset"), F("Id"), String(id), "");
						success = false;
					}
				}
				else if (node["obj"] == "ACTIONCHAIN") {
					id = (int)node["id"];
					if (id < ACTIONCHAINS_NUM) {
						success = actionchains[id]->deserializeJSON(node);
						LOGDEBUG(F("[FileSystem]"), F("loadSettings()"), F("OK: Loaded Actionchain"), F("Id"), String(id), "");
					}
					else {
						LOGDEBUG(F("[FileSystem]"), F("loadSettings()"), F("ERROR: Invalid Actionchain"), F("Id"), String(id), "");
						success = false;
					}
				}
				else if (node["obj"] == "RCSOCKET") {
					id = (int)node["id"];
					if (id < RC_SOCKETS) {
						success = rcsocketcontroller->deserializeJSON(id, node);
						LOGDEBUG(F("[FileSystem]"), F("loadSettings()"), F("OK: Loaded Remote Controlled Socket"), F("Id"), String(id), "");
					}
					else {
						LOGDEBUG(F("[FileSystem]"), F("loadSettings()"), F("ERROR: Remote Controlled Socket"), F("Id"), String(id), "");
						success = false;
					}
				}
			}
			else {
				LOGMSG(F("[FileSystem]"), F("ERROR: Parsing JSON @"), F("Line"), String(j), "");
				success = false;
			}
			j++;
		}
		file.close();
		LOGMSG(F("[FileSystem]"), F("OK: Loaded Settings from file"), String(filename), "Lines", String(j));
	}
	else {
		LOGMSG(F("[FileSystem]"), F("ERROR: Could not read from file:"), String(filename), "", "");
		success = false;
	}
	return success;
}

bool Setting::copyFile(const char* source, const char* destination)
{
	File backup_file;
	File current_file;

	size_t n;
	uint8_t buf[64];

	short i = 0;
	String output;
	bool success = true;

	if (current_file.open(source, O_READ)) {
		LOGDEBUG2(F("[FileSystem]"), F("copyFile()"), F("OK: Source File open"), F("Filename"), String(source), "");

		if (backup_file.open(destination, O_CREAT | O_TRUNC | O_WRITE)) {
			LOGDEBUG2(F("[FileSystem]"), F("copyFile()"), F("OK: Target File open"), F("Filename"), String(destination), "");

			led[2]->turnOn();
			while ((current_file.read(buf, sizeof(buf))) > 0) {
				led[2]->switchState();
				backup_file.write(buf, sizeof(buf));
				i++;
			}
			led[2]->turnOff();

			output = String(i * 64);
			current_file.close();
			backup_file.close();
			LOGMSG(F("[FileSystem]"), F("OK: Copied settings to backup file"), String(destination), "Filesize", String(output));
		}
		else {
			LOGMSG(F("[FileSystem]"), F("ERROR: Could not open destination file"), String(destination), "", "");
			success = false;
		}
	}
	else {
		LOGMSG(F("[FileSystem]"), F("ERROR: Could not open target file"), String(source), "", "");
		success = false;
	}

	return success;
}
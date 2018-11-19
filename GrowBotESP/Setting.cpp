// 
// 
// 

#include "Setting.h"

Setting::Setting(String active_config_file, String default_config_file, String backup_config_file, String log_file, String wifi_ssid, String wifi_pw, String ap_ssid, String ap_pw, String http_user, String http_password)
{
	//Set Initial Values
	this->active_config_file = active_config_file;
	this->default_config_file = default_config_file;
	this->backup_config_file = backup_config_file;
	this->log_file = log_file;
	this->wifi_ssid = wifi_ssid;
	this->wifi_pw = wifi_pw;
	this->ap_ssid = ap_ssid;
	this->ap_pw = ap_pw;
	this->http_user = http_user;
	this->http_pw = http_password;

	//Set Default Values
	this->default_wifi_ssid = this->wifi_ssid;
	this->default_wifi_pw = this->wifi_pw;
	this->default_ap_ssid = this->ap_ssid;
	this->default_ap_pw = this->ap_pw;
	this->default_http_user = this->http_user;
	this->default_http_pw = this->http_pw;
}

void Setting::begin()
{
	if (DEBUG_RESET == false) {
		if (loadActiveConfig() == false) {
			LOGMSG("[Setup]", "WARNING: Did not load primary config file", "Hardreset", DEBUG_RESET, "");
			String keys[] = { "" };
			String values[] = { "" };
			logengine.addLogEntry(WARNING, "Main", "Did not load primary config file", keys, values, 0);

			if (loadBackupConfig() == false) {
				LOGMSG("[Setup]", "WARNING: Did not load backup config file", "Hardreset", DEBUG_RESET, "");
				String keys[] = { "" };
				String values[] = { "" };
				logengine.addLogEntry(WARNING, "Main", "Did not load backup config file", keys, values, 0);

				if (loadDefaultConfig() == false) {
					LOGMSG("[Setup]", "WARNING: Did not load default config file", "Hardreset", DEBUG_RESET, "");
					String keys[] = { "" };
					String values[] = { "" };
					logengine.addLogEntry(WARNING, "Main", "Did not load default config file. Setting hard coded values", keys, values, 0);

					Setting::reset();
				}
			}
		}
	}
	else {
		LOGMSG("[Setup]", "WARNING: Hard Reset Flag set. Setting hard coded values", "Hardreset", DEBUG_RESET, "");
		String keys[] = { "" };
		String values[] = { "" };
		logengine.addLogEntry(WARNING, "Main", "Reset Flag set. Setting hard coded values", keys, values, 0);

		Setting::reset();
	}
}



void Setting::reset()
{
	//Set Network Default Values
	this->wifi_ssid = this->default_wifi_ssid;
	this->wifi_pw = this->default_wifi_pw;
	
	this->ap_ssid = this->default_ap_ssid;
	this->ap_pw = this->default_ap_pw;

	this->http_user = this->default_http_user;
	this->http_pw = this->default_http_pw;


	LOGMSG(F("[FileSystem]"), F("OK: Reset to Default Settings (SSID | Password | API_secret)"), String(ap_ssid), String(ap_pw), "");

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
	//rcsocketcontroller->resetSettings();

	String keys[] = { "Wifi SSID", "Wifi Password", "AP SSID", "AP Password", "Http User", "Http Password" };
	String values[] = { wifi_ssid, wifi_pw, ap_ssid, ap_pw, http_user, http_pw };
	logengine.addLogEntry(INFO, "Setting", "Reset to Default Settings", keys, values, 5);
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
	
	settings["ap_SSID"] = ap_ssid;
	settings["ap_pw"] = ap_pw;

	settings["http_user"] = http_user;
	settings["http_password"] = http_pw;

	settings["time"] = sensor_cycles * SENS_FRQ_SEC - internalRTC.timezone_offset;
	settings["timezone"] = internalRTC.timezone_offset;

	settings["log_size"] = logengine.counter;

	settings.printTo(json, maxSize);
	LOGDEBUG(F("[Setting]"), F("serializeJSON()"), F("OK: Serialized Settings"), String(settings.measureLength()), String(maxSize), "");
}

void Setting::serializeJSON(JsonObject & data)
{
	data["obj"] = "SETTING";
	//Constants
	data["firm_version"] = GROWBOT_FIRMWARE;
	data["firm_date"] = __DATE__;
	data["firm_time"] = __TIME__;
	data["actions_num"] = ACTIONS_NUM;
	data["actionschains_num"] = ACTIONCHAINS_NUM;
	data["actionschains_length"] = ACTIONCHAIN_LENGTH;
	data["rulesets_num"] = RULESETS_NUM;
	data["rulesets_trig"] = RULESETS_TRIGGER;
	data["rulesets_act"] = RULESETS_ACTIONS;
		
	data["trigger_sets"] = TRIGGER_SETS;
	data["trigger_types"] = TRIGGER_TYPES;
	data["sensor_num"] = SENS_NUM;
	data["task_queue_length"] = TASK_QUEUE_LENGTH;
	data["actionchain_task_maxduration"] = ACTIONCHAIN_TASK_MAXDURATION;
	data["task_parallel_sec"] = TASK_PARALLEL_SEC;
	data["rc_sockets_num"] = RC_SOCKETS;
	data["rc_signals_num"] = RC_SIGNALS;
	data["task_frq_sec"] = TASK_FRQ_SEC;
	data["sens_frq_sec"] = SENS_FRQ_SEC;

	//Variable Settings	
	data["wifi_SSID"] = wifi_ssid;
	data["wifi_pw"] = wifi_pw;

	data["ap_SSID"] = ap_ssid;
	data["ap_pw"] = ap_pw;

	data["http_user"] = http_user;
	data["http_password"] = http_pw;

	data["time"] = sensor_cycles * SENS_FRQ_SEC - internalRTC.timezone_offset;
	data["timezone"] = internalRTC.timezone_offset;

	data["log_size"] = logengine.counter;

	LOGDEBUG(F("[Setting]"), F("serializeJSON()"), F("OK: Serialized Overall Settings "), String(data.measureLength()), "", "");
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

		if (data["wifi_pw"].success()) {
			wifi_pw = data["wifi_pw"].asString();
			LOGMSG(F("[Setting]"), F("OK: Wifi SSID loaded"), String(wifi_pw), "", "");
		}
		else {
			LOGMSG(F("[Setting]"), F("ERROR: No Wifi password loaded"), "", "", "");
		}
		if (data["ap_SSID"].asString() != "") {
			ap_ssid = data["ap_SSID"].asString();
			LOGMSG(F("[Setting]"), F("OK: AP SSID loaded"), String(ap_ssid), "", "");
		}
		else {
			LOGMSG(F("[Setting]"), F("ERROR: No AP SSID loaded"), "", "", "");
		}

		if (data["ap_pw"].asString() != "") {
			ap_pw = data["ap_pw"].asString();
			LOGMSG(F("[Setting]"), F("OK: AP password loaded"), String(ap_pw), "", "");
		}
		else {
			LOGMSG(F("[Setting]"), F("ERROR: No AP password loaded"), "", "", "");
		}

		if (data["http_user"].asString() != "") {
			http_user = data["http_user"].asString();
			LOGMSG(F("[Setting]"), F("OK: Loaded http user"), String(http_user), "", "");
		}
		else {
			LOGMSG(F("[Setting]"), F("WARNING: No http user loaded"), "", "", "");
		}
		
		if (data["http_password"].success()) {
			http_pw = data["http_password"].asString();
			LOGMSG(F("[Setting]"), F("OK: Loaded http password"), "#", String(http_pw), "#");
		}
		else {
			LOGMSG(F("[Setting]"), F("WARNING: No http password loaded"), "", "", "");
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

bool Setting::saveActiveConfig()
{
	LOGDEBUG2(F("[FileSystem]"), F("saveSettings()"), F("OK: Saved Config to active config file"), "", "", "");
	return saveSettings(active_config_file.c_str());
}

bool Setting::saveDefaultConfig()
{
	LOGDEBUG2(F("[FileSystem]"), F("saveSettings()"), F("OK: Saved Config to default config file"), "", "", "");
	return saveSettings(default_config_file.c_str());
}


bool Setting::backupConfig()
{
	LOGDEBUG2(F("[FileSystem]"), F("copyFile()"), F("OK: Ending Task"), "", "", "");
	return copyFile(active_config_file.c_str(), backup_config_file.c_str());
}

void Setting::asyncSaveActiveConfig(void * _this)
{
	vTaskDelay(50);
	static_cast<Setting*>(_this)->backupConfig();
	static_cast<Setting*>(_this)->saveActiveConfig();
	vTaskDelete(NULL);
}

void Setting::asyncSaveDefaultConfig(void * _this)
{
	vTaskDelay(50);
	static_cast<Setting*>(_this)->saveDefaultConfig();
	vTaskDelete(NULL);
}

void Setting::asyncBackupConfig(void *_this)
{
	vTaskDelay(50);
	static_cast<Setting*>(_this)->backupConfig();
	vTaskDelete(NULL);
}

bool Setting::loadActiveConfig()
{
	return loadSettings(active_config_file.c_str());
}

bool Setting::loadDefaultConfig()
{
	return loadSettings(default_config_file.c_str());
}

bool Setting::loadBackupConfig()
{
	return loadSettings(backup_config_file.c_str());
}

bool Setting::saveSettings(const char*  filename)
{
	DynamicJsonBuffer jsonbuffer;

	File file = SD.open(filename, FILE_WRITE);

	if (file) {
		//LOGDEBUG2(F("[FileSystem]"), F("saveSettings()"), F("File Open"), "", "", "");
		led[2]->turnOn();
		
		//Settings
		JsonObject& data = jsonbuffer.createObject();
		serializeJSON(data);
		data.printTo(file);
		file.println();
		LOGDEBUG2(F("[FileSystem]"), F("saveSettings()"), F("OK: Saved Settings"), "", "", "");

		for (uint8_t i = 0; i < TRIGGER_TYPES; i++) {
			LOGDEBUG2(F("[Setting]"), F("saveSettings()"), F("Free HEAP before Trigger"), String(ESP.getFreeHeap()), "", "");
			for (uint8_t j = 0; j < TRIGGER_SETS; j++) {
				jsonbuffer.clear();
				JsonObject& data = jsonbuffer.createObject();
				trigger[i][j]->serializeJSON(data, DETAILS);
				data.printTo(file);
				file.println();
				led[2]->switchState();
			}
		}
		for (uint8_t i = 0; i < RULESETS_NUM; i++) {
			LOGDEBUG2(F("[Setting]"), F("saveSettings()"), F("Free HEAP before Ruleset"), String(ESP.getFreeHeap()), "", "");
			jsonbuffer.clear();
			JsonObject& data = jsonbuffer.createObject();
			rulesets[i]->serializeJSON(data, DETAILS);
			data.printTo(file);
			file.println();
			led[2]->switchState();
		}
		for (uint8_t i = 0; i < ACTIONCHAINS_NUM; i++) {
			LOGDEBUG2(F("[Setting]"), F("saveSettings()"), F("Free HEAP before Actionchain"), String(ESP.getFreeHeap()), "", "");
			jsonbuffer.clear();
			JsonObject& data = jsonbuffer.createObject();
			actionchains[i]->serializeJSON(data, DETAILS);
			data.printTo(file);
			file.println();
			led[2]->switchState();
		}
		for (uint8_t i = 0; i < SENS_NUM; i++) {
			LOGDEBUG2(F("[Setting]"), F("saveSettings()"), F("Free HEAP before Sensors"), String(ESP.getFreeHeap()), "", "");
			jsonbuffer.clear();
			JsonObject& data = jsonbuffer.createObject();
			sensors[i]->serializeJSON(data, DETAILS);
			data.printTo(file);
			file.println();
			led[2]->switchState();			
		}
		for (uint8_t i = 0; i < RC_SOCKETS; i++) {
			LOGDEBUG2(F("[Setting]"), F("saveSettings()"), F("Free HEAP before RC Sockets"), String(ESP.getFreeHeap()), "", "");
			jsonbuffer.clear();
			JsonObject& data = jsonbuffer.createObject();
			rcsocketcontroller->serializeJSON(data, DETAILS, i);
			data.printTo(file);
			file.println();
			led[2]->switchState();
		}
		jsonbuffer.clear();
		led[2]->turnOff();
		file.close();

		LOGMSG(F("[FileSystem]"), F("OK: Saved Settings to file:"), String(filename), "Free Heap", String(ESP.getFreeHeap()));
	}
	else {
		LOGMSG(F("[FileSystem]"), F("ERROR: Could not write to file:"), String(filename), "", "");
	}
}


bool Setting::loadSettings(const char* filename)
{
	String json;
	int cat = 0;
	int id = 0;
	int j = 0;
	bool success = true;

	File file = SD.open(filename, FILE_READ);

	if (file) {
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
	size_t n;
	uint8_t buf[64];

	short i = 0;
	String output;
	bool success = true;

	File current_file = SD.open(source, FILE_READ);

	if (current_file) {
		LOGDEBUG(F("[FileSystem]"), F("copyFile()"), F("OK: Source File open"), F("Filename"), String(source), "");

		File backup_file = SD.open(destination, FILE_WRITE);

		if (backup_file) {
			LOGDEBUG(F("[FileSystem]"), F("copyFile()"), F("OK: Target File open"), F("Filename"), String(destination), "");

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
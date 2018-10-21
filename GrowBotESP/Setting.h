// Setting.h

#ifndef _SETTING_h
#define _SETTING_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
//Helper
#include "Definitions.h"
#include <ArduinoJson.h>

//Hardware
#include <SD.h>
#include "Led.h"
#include "RealTimeClock.h"

#include "LogEngine.h"

//Control Objects
#include "AdvancedSensor.h"
#include "Trigger.h"
#include "Ruleset.h"
#include "ActionChain.h"
#include "RCSocketController.h"

//Settings Globals
extern String wifi_ssid;
extern String wifi_pw;
extern const char* http_user;
extern const char* http_password;
extern long sensor_cycles;

//Hardware Globals
extern Led *led[3];
extern RealTimeClock internalRTC;

extern LogEngine logengine;

//Control Objects Globals
extern SensorInterface *sensors[SENS_NUM];
extern Trigger *trigger[TRIGGER_TYPES][TRIGGER_SETS];
extern RuleSet *rulesets[RULESETS_NUM];
extern ActionChain *actionchains[ACTIONCHAINS_NUM];
extern RCSocketController *rcsocketcontroller;

class Setting {
public:
	String active_config_file;
	String default_config_file;
	String backup_config_file;
	String log_file;



	String wifi_ssid;
	String wifi_pw;
	String ap_ssid;
	String ap_pw;
	String http_user;
	String http_pw;

	String default_wifi_ssid;
	String default_wifi_pw;

	String default_ap_ssid;
	String default_ap_pw;

	String default_http_user;
	String default_http_pw;

	
	Setting(String active_config_file, String default_config_file, String backup_config_file, String log_file, String wifi_ssid, String wifi_pw, String ap_ssid, String ap_pw, String http_user, String http_password);

	void reset();

	void begin();

	//Save Operations
	bool saveActiveConfig();
	bool saveDefaultConfig();
	bool backupConfig();

	//Async Wrapper Functions
	static void asyncSaveActiveConfig(void*);
	static void asyncSaveDefaultConfig(void*);
	static void asyncBackupConfig(void*);
	
	bool loadActiveConfig();
	bool loadDefaultConfig();
	bool loadBackupConfig();

	void serializeJSON(char* json, size_t maxSize);
	void serializeJSON(JsonObject& data);
	bool deserializeJSON(JsonObject& data);

	//Base Methods
	bool loadSettings(const char* filename);
	bool saveSettings(const char* filename);

	bool copyFile(const char* source, const char* destination);


};


#endif

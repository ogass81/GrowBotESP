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
#include "Sensor.h"
#include "Trigger.h"
#include "Ruleset.h"
#include "ActionChain.h"
#include "RCSocketController.h"

//Settings Globals
extern String wifi_ssid;
extern String wifi_pw;
extern String api_secret;
extern long sensor_cycles;

//Hardware Globals
extern Led *led[3];
extern RealTimeClock internalRTC;

extern LogEngine logengine;

//Control Objects Globals
extern Sensor *sensors[SENS_NUM];
extern Trigger *trigger[TRIGGER_TYPES][TRIGGER_SETS];
extern RuleSet *rulesets[RULESETS_NUM];
extern ActionChain *actionchains[ACTIONCHAINS_NUM];
extern RCSocketController *rcsocketcontroller;

class Setting {
public:

	static void reset();

	static void serializeJSON(char* json, size_t maxSize);
	static bool deserializeJSON(JsonObject& data);

	static bool loadSettings(const char* filename);
	static bool saveSettings(const char* filename);
	static bool copyFile(const char* source, const char* destination);
};


#endif

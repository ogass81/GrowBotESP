// WebHandler.h

#ifndef _WEBHANDLER_h
#define _WEBHANDLER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Definitions.h"
#include <ESPAsyncWebServer.h>
#include <AsyncJson.h>

#include "RealTimeClock.h"
#include "AdvancedSensor.h"
#include "Trigger.h"
#include "Ruleset.h"
#include "Action.h"
#include "ActionChain.h"
#include "RCSocketController.h"
#include "Setting.h"
#include "TaskManager.h"
#include "LogEngine.h"

extern const char* http_user;
extern const char* http_password;

extern SensorInterface *sensors[SENS_NUM];
extern Trigger *trigger[TRIGGER_TYPES][TRIGGER_SETS];
extern RuleSet *rulesets[RULESETS_NUM];
extern Action *actions[ACTIONS_NUM];
extern ActionChain *actionchains[ACTIONCHAINS_NUM];
extern RCSocketController *rcsocketcontroller;
extern TaskManager *taskmanager;
extern LogEngine logengine;
extern Setting settings;

class Webhandler {
public:
	AsyncWebServer classWebServer = AsyncWebServer(80);
	
	Webhandler();
	
	void begin();

	void loginGet(AsyncWebServerRequest *request);
	void actionGet(AsyncWebServerRequest *request);
	void actionchainGet(AsyncWebServerRequest *request);
	void logGet(AsyncWebServerRequest *request);
	void rcsocketGet(AsyncWebServerRequest *request);
	void rulesetGet(AsyncWebServerRequest *request);
	void sensorGet(AsyncWebServerRequest *request);
	void settingGet(AsyncWebServerRequest *request);
	void triggerGet(AsyncWebServerRequest *request);

	void unknownGet(AsyncWebServerRequest *request);


	void actionchainPatch(AsyncWebServerRequest *request, JsonVariant &json);
	void rcsocketPatch(AsyncWebServerRequest *request, JsonVariant &json);
	void rulesetPatch(AsyncWebServerRequest *request, JsonVariant &json);
	void sensorPatch(AsyncWebServerRequest *request, JsonVariant &json);
	void settingPatch(AsyncWebServerRequest *request, JsonVariant &json);
	void triggerPatch(AsyncWebServerRequest *request, JsonVariant &json);


	void breakupURL(String uri[REST_URI_DEPTH], char * url);
};


#endif


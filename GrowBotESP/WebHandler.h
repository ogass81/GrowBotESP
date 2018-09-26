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
#include "Sensor.h"
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

extern Sensor *sensors[SENS_NUM];
extern Trigger *trigger[TRIGGER_TYPES][TRIGGER_SETS];
extern RuleSet *rulesets[RULESETS_NUM];
extern Action *actions[ACTIONS_NUM];
extern ActionChain *actionchains[ACTIONCHAINS_NUM];
extern RCSocketController *rcsocketcontroller;
extern TaskManager *taskmanager;
extern LogEngine logengine;

class Webhandler {
public:
	AsyncWebServer classWebServer = AsyncWebServer(80);
	
	Webhandler();


	void begin();

	void loginRequest(AsyncWebServerRequest *request);
	void actionRequest(AsyncWebServerRequest *request);
	void actionchainRequest(AsyncWebServerRequest *request);
	void logRequest(AsyncWebServerRequest *request);
	void rcsocketRequest(AsyncWebServerRequest *request);
	void rulesetRequest(AsyncWebServerRequest *request);
	void sensorRequest(AsyncWebServerRequest *request);
	void settingRequest(AsyncWebServerRequest *request);
	void triggerRequest(AsyncWebServerRequest *request);

	void breakupURL(String uri[REST_URI_DEPTH], char * url);
};


#endif


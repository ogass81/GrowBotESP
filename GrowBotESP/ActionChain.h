// ActionChain.h

#ifndef _ACTIONCHAIN_h
#define _ACTIONCHAIN_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#include "Definitions.h"
#include <ArduinoJson.h>
#include "Action.h"
#include "TaskManager.h"

class TaskManager;

extern TaskManager *taskmanager;
extern Action *actions[ACTIONS_NUM];


class ActionChain {
public:
	uint8_t id;
	bool active;
	String title;

	//Parameter for Action Objects
	uint8_t actionPtr[ACTIONCHAIN_LENGTH];
	uint8_t actionPar[ACTIONCHAIN_LENGTH];

	//Pointer to Action Objects
	Action *assignedAction[ACTIONCHAIN_LENGTH];

	ActionChain(int count);

	//UI Output
	String getTitle();

	//Settings
	void reset();

	//Serialize
	void serializeJSON(uint8_t id, char* json, size_t maxSize, Scope scope);
	bool deserializeJSON(JsonObject& data);

	//Execute all Actions in Chain
	void execute();
};

#endif


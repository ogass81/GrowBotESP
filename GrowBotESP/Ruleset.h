// Ruleset.h

#ifndef _NUM_RULESETS_NUMETSET_h
#define _NUM_RULESETS_NUMETSET_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#include "Definitions.h"
#include <ArduinoJson.h>
#include "Trigger.h"
#include "ActionChain.h"

#include "LogEngine.h"
extern LogEngine logengine;

extern ActionChain *actionchains[ACTIONCHAINS_NUM];
extern Trigger *trigger[TRIGGER_TYPES][TRIGGER_SETS];

//Rulesets are Touples of Triggers, Boolean Operators. If the whole expression is true the assigned action (callback function) is executed
class RuleSet {
public:
	uint8_t id;
	String title;
	bool active = false;

	//Pointer Values for Serialization
	uint8_t triggerPtr[RULESETS_TRIGGER][2];
	uint8_t actionChainPtr[RULESETS_ACTIONS];

	Trigger *assignedTrigger[RULESETS_TRIGGER];
	ActionChain *assignedChain[RULESETS_ACTIONS];
	BoolOp assignedBoolOp[RULESETS_TRIGGER - 1];

	RuleSet(uint8_t id);

	//UI Output
	String getTitle();


	//Check State
	bool checkState();

	//Settings
	void reset();

	//Serialize
	void serializeJSON(JsonObject& data, Scope scope);
	bool deserializeJSON(JsonObject& data);

	void execute();
};

#endif


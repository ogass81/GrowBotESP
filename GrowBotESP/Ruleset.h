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
	uint8_t triggercat1_ptr, triggercat2_ptr, triggercat3_ptr;
	uint8_t triggerset1_ptr, triggerset2_ptr, triggerset3_ptr;
	uint8_t chain_ptr;

	Trigger *assignedTrigger[3];
	BoolOp assignedBoolOp[2];
	ActionChain *assignedChain;

	RuleSet(int count);

	//UI Output
	String getTitle();


	//Check State
	bool checkState();

	//Settings
	void reset();

	//Serialize
	void serializeJSON(uint8_t id, char* json, size_t maxSize, Scope scope);
	bool deserializeJSON(JsonObject& data);

	void execute();
};

#endif


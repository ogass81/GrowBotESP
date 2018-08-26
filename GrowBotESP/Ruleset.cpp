// 
// 
// 

#include "Ruleset.h"

RuleSet::RuleSet(int count)
{
	id = count;
	reset();
}

String RuleSet::getTitle()
{
	return String(title);
}

bool RuleSet::checkState()
{
	bool state = false;
	uint8_t i = 0;

	if (active == true) {
		while (assignedTrigger[i] != NULL) {
			if (i == 0) {
				state = assignedTrigger[i]->checkState();
				LOGDEBUG(F("[Ruleset]"), F("checkstate()"), F("OK: First Trigger"), String(assignedTrigger[i]->getTitle()), String(state), "");
			}
			else {
				if (assignedTrigger[i - 1] != NULL) {

					switch (assignedBoolOp[i - 1]) {

					case AND:
						state = state && assignedTrigger[i]->checkState();
						LOGDEBUG(F("[Ruleset]"), F("checkstate()"), F("OK: AND Next Trigger"), String(assignedTrigger[i]->getTitle()), String(state), "");
						break;

					case OR:
						state = state || assignedTrigger[i]->checkState();
						LOGDEBUG(F("[Ruleset]"), F("checkstate()"), F("OK: OR Next Trigger"), String(assignedTrigger[i]->getTitle()), String(state), "");
						break;

					case NOT:
						state = state && !assignedTrigger[i]->checkState();
						LOGDEBUG(F("[Ruleset]"), F("checkstate()"), F("OK: NOT Next Trigger"), String(assignedTrigger[i]->getTitle()), String(state), "");
						break;
					}
				}
				else LOGDEBUG(F("[Ruleset]"), F("checkstate()"), F("ERROR: No previous Trigger in Ruleset"), F("Final State @"), String(i), String(state));
			}
			i++;
		}
	}
	return state;
}

void RuleSet::reset()
{
	title = "Ruleset ";
	title += String(id);
	active = false;

	assignedTrigger[0] = NULL;
	assignedTrigger[1] = NULL;
	assignedTrigger[2] = NULL;

	assignedBoolOp[0] = AND;
	assignedBoolOp[1] = AND;

	assignedChain = NULL;

	triggercat1_ptr = TRIGGER_TYPES;
	triggercat2_ptr = TRIGGER_TYPES;
	triggercat3_ptr = TRIGGER_TYPES;
	triggerset1_ptr = TRIGGER_SETS;
	triggerset2_ptr = TRIGGER_SETS;
	triggerset3_ptr = TRIGGER_SETS;

	chain_ptr = ACTIONCHAINS_NUM;
}

void RuleSet::serializeJSON(uint8_t id, char * json, size_t maxSize, Scope scope)
{
	StaticJsonBuffer<JSONBUFFER_SIZE> jsonBuffer;

	JsonObject& rules = jsonBuffer.createObject();

	if (scope == LIST || scope == DETAILS) {
		rules["tit"] = title;
		rules["act"] = active;
	}

	if (scope == DETAILS) {
		rules["id"] = id;
		rules["obj"] = "RULESET";
		rules["tcat1_ptr"] = triggercat1_ptr;
		rules["tset1_ptr"] = triggerset1_ptr;
		rules["tcat2_ptr"] = triggercat2_ptr;
		rules["tset2_ptr"] = triggerset2_ptr;
		rules["tcat3_ptr"] = triggercat3_ptr;
		rules["tset3_ptr"] = triggerset3_ptr;
		rules["chain_ptr"] = chain_ptr;

		JsonArray& boolop = rules.createNestedArray("bool");
		boolop.add(static_cast<int>(assignedBoolOp[0]));
		boolop.add(static_cast<int>(assignedBoolOp[1]));
	}

	rules.printTo(json, maxSize);
	LOGDEBUG2(F("[Ruleset]"), F("serializeJSON()"), F("OK: Serialized Members for Ruleset"), String(id), String(rules.measureLength()), String(maxSize));
}

bool RuleSet::deserializeJSON(JsonObject & data)
{
	if (data.success() == true) {
		if (data["tit"] != "") title = data["tit"].asString();
		if (data["act"] != "") active = data["act"];
		if (data["tset1_ptr"] != "") triggerset1_ptr = data["tset1_ptr"];
		if (data["tcat1_ptr"] != "") triggercat1_ptr = data["tcat1_ptr"];
		if (data["tset2_ptr"] != "") triggerset2_ptr = data["tset2_ptr"];
		if (data["tcat2_ptr"] != "") triggercat2_ptr = data["tcat2_ptr"];
		if (data["tset3_ptr"] != "") triggerset3_ptr = data["tset3_ptr"];
		if (data["tcat3_ptr"] != "") triggercat3_ptr = data["tcat3_ptr"];
		if (data["chain_ptr"] != "") chain_ptr = data["chain_ptr"];
		
		//Assigning Pointers
		
		if (data["bool"][0] != "") {
			if (data["bool"][0] == 0) assignedBoolOp[0] = AND;
			else if (data["bool"][0] == 1) assignedBoolOp[0] = OR;
			else if (data["bool"][0] == 2) assignedBoolOp[0] = NOT;
			else {
				assignedBoolOp[0] = OR;
				active = false;
			}
		}

		if (data["bool"][1] != "") {
			if (data["bool"][1] == 0) assignedBoolOp[1] = AND;
			else if (data["bool"][1] == 1) assignedBoolOp[1] = OR;
			else if (data["bool"][1] == 2) assignedBoolOp[1] = NOT;
			else {
				assignedBoolOp[1] = OR;
				active = false;
			}
		}

		if (triggerset1_ptr != TRIGGER_SETS && triggercat1_ptr != TRIGGER_TYPES) assignedTrigger[0] = trigger[triggercat1_ptr][triggerset1_ptr];
		else assignedTrigger[0] = NULL;
		if (triggerset2_ptr != TRIGGER_SETS && triggercat2_ptr != TRIGGER_TYPES) assignedTrigger[1] = trigger[triggercat2_ptr][triggerset2_ptr];
		else assignedTrigger[1] = NULL;
		if (triggerset3_ptr != TRIGGER_SETS && triggercat3_ptr != TRIGGER_TYPES) assignedTrigger[2] = trigger[triggercat3_ptr][triggerset3_ptr];
		else assignedTrigger[2] = NULL;
		if (chain_ptr != ACTIONCHAINS_NUM) assignedChain = actionchains[chain_ptr];
		else assignedChain = NULL;

		LOGDEBUG2(F("[Ruleset]"), F("deserializeJSON()"), F("OK: Deserialized members for Ruleset"), data["id"].asString(), F("Datasize"), String(data.size()));
	}
	else {
		LOGDEBUG2(F("[Ruleset]"), F("deserializeJSON()"), F("ERROR: No Data to deserialize membersRuleset"), F("Datasize"), String(data.size()), "");
	}
	return data.success();
}

void RuleSet::execute()
{
	if (checkState() == true) {
		if(assignedChain != NULL) {
			assignedChain->execute();
		}
	}
}
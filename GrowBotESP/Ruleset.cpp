// 
// 
// 

#include "Ruleset.h"

RuleSet::RuleSet(uint8_t id)
{
	this->id = id;
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
		while (i < RULESETS_TRIGGER && assignedTrigger[i] != NULL) {   
			if (i == 0) {
				state = assignedTrigger[i]->checkState();
				LOGDEBUG(F("[Ruleset]"), F("checkstate()"), F("INITIAL Trigger "),  String(i), String(assignedTrigger[i]->getTitle()), String(state));
			}
			else {
				if (assignedTrigger[i - 1] != NULL) {
					
					switch (assignedBoolOp[i - 1]) {

					case AND:
					
						state = (state && assignedTrigger[i]->checkState());
						LOGDEBUG(F("[Ruleset]"), F("checkstate()"), F("AND Trigger "), String(i), String(assignedTrigger[i]->getTitle()), String(state));
						break;

					case OR:
					
						state = (state || assignedTrigger[i]->checkState());
						LOGDEBUG(F("[Ruleset]"), F("checkstate()"), F("OR Trigger "), String(i), String(assignedTrigger[i]->getTitle()), String(state));
						break;

					case NOT:
					
						state = (state && !assignedTrigger[i]->checkState());
						LOGDEBUG(F("[Ruleset]"), F("checkstate()"), F("NOT Trigger "), String(i), String(assignedTrigger[i]->getTitle()), String(state));
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

	for (uint8_t i = 0; i < RULESETS_TRIGGER; i++) {
		triggerPtr[i][0] = TRIGGER_TYPES;
		triggerPtr[i][1] = TRIGGER_SETS;
		assignedTrigger[i] = NULL;
	}

	for (uint8_t i = 0; i < RULESETS_TRIGGER-1; i++) {
		assignedBoolOp[i] = AND;
	}

	for (uint8_t i = 0; i < RULESETS_ACTIONS; i++) {
		actionChainPtr[i] = ACTIONCHAINS_NUM;
		assignedChain[i] = NULL;
	}

	LOGDEBUG3(F("[Ruleset]"), F("reset()"), F("OK: Reset Ruleset"), String(title), "", "");
}

void RuleSet::serializeJSON(JsonObject & data, Scope scope)
{
	if (scope == LIST || scope == DETAILS) {
		data["tit"] = title;
		data["act"] = active;
		data["state"] = checkState();
	}

	if (scope == DETAILS) {
		data["id"] = id;
		data["obj"] = "RULESET";
		
		JsonArray& trigger = data.createNestedArray("trigger");

		for (uint8_t i = 0; i < RULESETS_TRIGGER; i++) {
			JsonObject& set = trigger.createNestedObject();
			set["cat"] = triggerPtr[i][0];
			set["id"] = triggerPtr[i][1];
		}

		JsonArray& boolop = data.createNestedArray("boolop");
		for (uint8_t i = 0; i < RULESETS_TRIGGER - 1; i++) {
			boolop.add(static_cast<int>(assignedBoolOp[i]));
		}
	
		JsonArray& actionchain = data.createNestedArray("actionchain");
		for (uint8_t i = 0; i < RULESETS_ACTIONS; i++) {
			actionchain.add(actionChainPtr[i]);
		}
	}

	LOGDEBUG2(F("[Ruleset]"), F("serializeJSON()"), F("OK: Serialized Members of Ruleset"), String(id), String(this->getTitle()), String(data.measureLength()));
}

bool RuleSet::deserializeJSON(JsonObject & data)
{
	if (data.success() == true) {
		if (data["tit"] != "") title = data["tit"].asString();
		if (data["act"] != "") active = data["act"];


		for (uint8_t j = 0; j < RULESETS_TRIGGER; j++) {
			if (data["trigger"][j]["cat"] != "" && data["trigger"][j]["id"] != "" && data["trigger"][j]["cat"] < TRIGGER_TYPES && data["trigger"][j]["id"] < TRIGGER_SETS) {
				triggerPtr[j][0] = data["trigger"][j]["cat"];
				triggerPtr[j][1] = data["trigger"][j]["id"];
				assignedTrigger[j] = trigger[triggerPtr[j][0]][triggerPtr[j][1]];
			}
			else {
				triggerPtr[j][0] = TRIGGER_TYPES;
				triggerPtr[j][1] = TRIGGER_SETS;
				assignedTrigger[j] = NULL;
			}
		}

		for (uint8_t j = 0; j < RULESETS_TRIGGER-1; j++) {
			if (data["boolop"][j] != "") {
				switch ((int)data["boolop"][j]) {
					case 0:
						assignedBoolOp[j] = AND;
					break;

					case 1:
						assignedBoolOp[j] = OR;
						break;

					case 2:
						assignedBoolOp[j] = NOT;
						break;

					default:
						assignedBoolOp[j] = AND;
						active = false;
						break;
				}
			}
			else {
				assignedBoolOp[j] = AND;
			}
		}
		for (uint8_t j = 0; j < RULESETS_ACTIONS; j++) {
			if (data["actionchain"][j] != "" && data["actionchain"][j] < ACTIONCHAINS_NUM) {
				actionChainPtr[j] = data["actionchain"][j];
				assignedChain[j] = actionchains[actionChainPtr[j]];
			}
			else {
				actionChainPtr[j] = ACTIONCHAINS_NUM;
				assignedChain[j] = NULL;
			}
		}

		LOGDEBUG2(F("[Ruleset]"), F("deserializeJSON()"), F("OK: Deserialized Members of Ruleset"), String(id), String(this->getTitle()), String(data.size()));
	}
	else {
		LOGDEBUG2(F("[Ruleset]"), F("deserializeJSON()"), F("ERROR: No Data to deserialize membersRuleset"), String(id), String(this->getTitle()), String(data.size()));
	}
	return data.success();
}

void RuleSet::execute()
{
	if (checkState() == true) {
		for (uint8_t i = 0; i < RULESETS_ACTIONS; i++) {
			if (assignedChain[i] != NULL) {
				assignedChain[i]->execute();
			}
		}
	}
}

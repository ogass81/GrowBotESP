// 
// 
// 

#include "ActionChain.h"

uint8_t ActionChain::getActiveSlots()
{
	uint8_t counter = 0;
	for (uint8_t i = 0; i < ACTIONCHAIN_LENGTH; i++) {
		if (assignedAction[i] != NULL) counter++;
	}

	return uint8_t(counter);
}

String ActionChain::getActiveActions()
{
	String titles = "";
	for (uint8_t i = 0; i < ACTIONCHAIN_LENGTH; i++) {
		if (assignedAction[i] != NULL) titles += String(i) + ":" + assignedAction[i]->getTitle() +", ";
	}

	return String(titles);
}

ActionChain::ActionChain(int count)
{
	this->id = count;
	reset();
}

void ActionChain::reset()
{
	this->title = "Sequence " + String(this->id);
	active = false;

	for (uint8_t i = 0; i < ACTIONCHAIN_LENGTH; i++) {
		assignedAction[i] = NULL;
		actionPtr[i] = ACTIONS_NUM;
		actionPar[i] = 0;
	}

	LOGDEBUG(F("[ActionChain]"), F("reset()"), F("OK: Reset"), String(id), String(this->getTitle()), "");
}

void ActionChain::serializeJSON(JsonObject & data, Scope scope)
{
	if (scope == LIST || scope == DETAILS) {
		data["tit"] = title;
		data["act"] = active;
	}

	if (scope == DETAILS) {
		data["id"] = id;
		data["obj"] = "ACTIONCHAIN";

		JsonArray& ptr = data.createNestedArray("actptr");

		for (uint8_t i = 0; i < ACTIONCHAIN_LENGTH; i++) {
			ptr.add(actionPtr[i]);
		}

		JsonArray& par = data.createNestedArray("actpar");

		for (uint8_t i = 0; i < ACTIONCHAIN_LENGTH; i++) {
			par.add(actionPar[i]);
		}

	}
	LOGDEBUG2(F("[ActionChain]"), F("serializeJSON()"), F("OK: Serialized members"), String(this->title), String(data.measureLength()), static_cast<int>(scope));
}

bool ActionChain::deserializeJSON(JsonObject & data)
{
	if (data.success() == true) {
		
		if (data["tit"] != "") title = data["tit"].asString();
		if (data["act"] != "") active = data["act"];
		
		for (uint8_t i = 0; i < ACTIONCHAIN_LENGTH; i++) {	
			//Assign Pointers to Action using Index to Action
			if (data["actptr"][i] != "") {
				actionPtr[i] = data["actptr"][i];
			}
			else {
				actionPtr[i] = ACTIONS_NUM;
			}

			for (uint8_t i = 0; i < ACTIONCHAIN_LENGTH; i++) {
				if (actionPtr[i] != ACTIONS_NUM) {
					assignedAction[i] = actions[actionPtr[i]];
				} 
				else {
					assignedAction[i] = NULL;
				}
			}		
			//Assign Action Parameter to Member		
			if (data["actpar"][i] != "") actionPar[i] = data["actpar"][i];
			else actionPar[i] = 0;
		}

		LOGDEBUG2(F("[ActionChain]"), F("serializeJSON()"), F("OK: Serialized members"), F("ActionChain"), String(this->title), String(data.size()));
	}
	else {
		LOGDEBUG2(F("[ActionChain]"), F("deserializeJSON()"), F("ERROR: No Data to deserialize members"), F("ActionChain"), String(this->title), String(data.size()));
	}
	return data.success();
}

void ActionChain::execute()
{
	if (active == true) {
		taskmanager->addActions(this);

		String count = String(getActiveSlots());
		String titles = String(getActiveActions());

		String keys[] = { "Number:", "Actions:" };
		String values[] = { count, titles };
		logengine.addLogEntry(ACTION, "Sequence", String("Executed Sequence " + this->getTitle()), keys, values, 2);
		
		LOGDEBUG(F("[ActionChain]"), F("execute()"), F("OK: Actionchain sent to Task Manager"), F("ActionChain"), String(this->getTitle()), "");
	}
	else LOGDEBUG(F("[ActionChain]"), F("execute()"), F("ERROR: Actionchain not active"), F("ActionChain"), String(this->getTitle()), "");
}

String ActionChain::getTitle()
{
	return String(title);
}
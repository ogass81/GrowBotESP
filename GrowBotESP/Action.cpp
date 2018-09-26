// 
// 
// 

#include "Action.h"

void Action::execute()
{
}

void Action::serializeJSON(uint8_t id, char * json, size_t maxSize, Scope scope)
{
}

void Action::serializeJSON(JsonObject & data, Scope scope)
{
}

String Action::getTitle()
{
	//return String(group_title + " " + title);
	return String("falsche");
}

void Action::setAntagonist(String group_title, Action * aObject)
{
	LOGDEBUG2(F("[Action]"), F("setAntagonist()"), F("OK: Setting Antagonist Object in Baseclass"), "", "", "");
	this->group_title = group_title;
	this->antaObject = aObject;
}


template<class ActionType>
SimpleAction<ActionType>::SimpleAction(uint8_t id, String title, ActionType * actionObj, void(ActionType::*cFunct)(), bool visible)
{
	this->id = id;
	this->title = title;
	this->group_title = "";
	this->actionObject = actionObj;
	this->callback = cFunct;
	this->visible = visible;
}

template<class ActionType>
void SimpleAction<ActionType>::serializeJSON(uint8_t id, char * json, size_t maxSize, Scope scope)
{
	StaticJsonBuffer<500> jsonBuffer;

	JsonObject& actions = jsonBuffer.createObject();

	if (scope == LIST || scope == DETAILS) {
		actions["tit"] = title;
		actions["grp"] = group_title;
		if(antaObject != NULL) actions["opp"] = antaObject->title;
		actions["vis"] = visible;
	}
	
	if (scope == DETAILS) {
		actions["id"] = id;
		actions["obj"] = "ACTION";
	}

	actions.printTo(json, maxSize);
	LOGDEBUG2(F("[SimpleAction]"), F("serializeJSON()"), F("OK: Serialized Action"), String(id), String(actions.measureLength()), String(maxSize));
}

template<class ActionType>
void SimpleAction<ActionType>::serializeJSON(JsonObject & data, Scope scope)
{
	if (scope == LIST || scope == DETAILS) {
		data["tit"] = title;
		data["grp"] = group_title;
		if (antaObject != NULL) data["opp"] = antaObject->title;
		data["vis"] = visible;
	}

	if (scope == DETAILS) {
		data["id"] = id;
		data["obj"] = "ACTION";
	}

	LOGDEBUG2(F("[SimpleAction]"), F("serializeJSON()"), F("OK: Serialized Action"), String(data.measureLength()), "", "");
}

template<class ActionType>
void SimpleAction<ActionType>::execute()
{
	if (actionObject != NULL) {
		if(callback != NULL) {
			LOGDEBUG(F("[Action]"), F("execute()"), F("OK: Execute Action"), getTitle(), "", "");
			(actionObject->*callback)();

			String keys[] = { "Action" };
			String values[] = { getTitle() };
			logengine.addLogEntry(ACTION, "Action", "OK: Execute Action", keys, values, 1);
		}
		else LOGDEBUG(F("[Action]"), F("execute()"), F("ERROR: Callback missing"), getTitle(), "", "");
	}
	else LOGDEBUG(F("[Action]"), F("execute()"), F("ERROR: Action Object missing"), getTitle(), "", "");
}

template<class ActionType>
String SimpleAction<ActionType>::getTitle()
{
	return String(this->title);
}

template<class ActionType>
ParameterizedSimpleAction<ActionType>::ParameterizedSimpleAction(uint8_t id, String title, ActionType * actionObj, void(ActionType::*cFunct)(int), int par, bool visible)
{
	this->id = id;
	this->title = title;
	this->group_title = "";
	this->actionObject = actionObj;
	this->callback = cFunct;
	this->visible = visible;
	this->parameter = par;
}

template<class ActionType>
void ParameterizedSimpleAction<ActionType>::serializeJSON(uint8_t id, char * json, size_t maxSize, Scope scope)
{
	StaticJsonBuffer<JSONBUFFER_SIZE> jsonBuffer;

	JsonObject& actions = jsonBuffer.createObject();

	if (scope == LIST || scope == DETAILS) {
		actions["tit"] = title;
		actions["grp"] = group_title;
		if (antaObject != NULL) actions["anta"] = antaObject->getTitle();
		actions["vis"] = visible;
		actions["par"] = parameter;
	}


	if (scope == DETAILS) {
		actions["id"] = id;
		actions["obj"] = "ACTION";
	}

	actions.printTo(json, maxSize);
	LOGDEBUG2(F("[ParameterizedSimpleAction]"), F("serializeJSON()"), F("OK: Serialized Action"), String(id), String(actions.measureLength()), String(maxSize));
}

template<class ActionType>
void ParameterizedSimpleAction<ActionType>::serializeJSON(JsonObject & data, Scope scope)
{
	if (scope == LIST || scope == DETAILS) {

		data["tit"] = title;
		data["grp"] = group_title;
		if (antaObject != NULL) data["anta"] = antaObject->getTitle();
		data["vis"] = visible;
		data["par"] = parameter;
	}

	if (scope == DETAILS) {
		data["id"] = id;
		data["obj"] = "ACTION";
	}

	LOGDEBUG2(F("[ParameterizedSimpleAction]"), F("serializeJSON()"), F("OK: Serialized Action"), String(data.measureLength()), "", "");
}

template<class ActionType>
void ParameterizedSimpleAction<ActionType>::execute()
{
	if (actionObject != NULL) {
		if (callback != NULL) {
			if (parameter >= 0) {
				LOGDEBUG(F("[Action]"), F("execute()"), F("OK: Execute Action"), getTitle(), "", "");
				String keys[] = { "Action" };
				String values[] = { getTitle() };
				logengine.addLogEntry(ACTION, "Action", "OK: Execute Action", keys, values, 1);

				(actionObject->*callback)(parameter);
			}
			else {
				LOGDEBUG(F("[Action]"), F("execute()"), F("ERROR: Argument missing"), getTitle(), "", "");

				String keys[] = { "Action" };
				String values[] = { title };
				logengine.addLogEntry(ACTION, "Action", "ERROR: Argument missing", keys, values, 1);
			}
		}
		else LOGDEBUG(F("[Action]"), F("execute()"), F("ERROR: Callback missing"), getTitle(), "", "");
	}
	else LOGDEBUG(F("[Action]"), F("execute()"), F("ERROR: Action Object missing"), getTitle(), "", "");
}

template<class ActionType>
String ParameterizedSimpleAction<ActionType>::getTitle()
{
	return String(title);
}

template<class ActionType>
NamedParameterizedSimpleAction<ActionType>::NamedParameterizedSimpleAction(uint8_t id, String title, ActionType * actionObj, void(ActionType::* cFunct)(int), String(ActionType::* getTitle)(int), int par, bool visible) :
ParameterizedSimpleAction<ActionType>(id, title, actionObj, cFunct, par, visible)
{
	this->getForeignTitle = getTitle;
}


template<class ActionType>
void NamedParameterizedSimpleAction<ActionType>::serializeJSON(uint8_t id, char * json, size_t maxSize, Scope scope)
{
	StaticJsonBuffer<JSONBUFFER_SIZE> jsonBuffer;

	JsonObject& actions = jsonBuffer.createObject();

	if (scope == LIST || scope == DETAILS) {
		String temp = this->title;
		temp += " ";
		temp += (this->actionObject->*getForeignTitle)(this->parameter);

		actions["tit"] = temp;
		actions["grp"] = this->group_title;
		if (this->antaObject != NULL) actions["anta"] = this->antaObject->getTitle();
		actions["vis"] = this->visible;
		actions["par"] = this->parameter;
	}


	if (scope == DETAILS) {
		actions["id"] = id;
		actions["obj"] = "ACTION";
	}

	actions.printTo(json, maxSize);
	LOGDEBUG2(F("[ParameterizedSimpleAction]"), F("serializeJSON()"), F("OK: Serialized Action"), String(id), String(actions.measureLength()), String(maxSize));
}

template<class ActionType>
void NamedParameterizedSimpleAction<ActionType>::serializeJSON(JsonObject & data, Scope scope)
{
	if (scope == LIST || scope == DETAILS) {
		String temp = this->title;
		temp += " ";
		temp += (this->actionObject->*getForeignTitle)(this->parameter);
		data["tit"] = temp;
		data["grp"] = this->group_title;
		if (this->antaObject != NULL) data["anta"] = this->antaObject->getTitle();
		data["vis"] = this->visible;
		data["par"] = this->parameter;
	}

	if (scope == DETAILS) {
		data["id"] = this->id;
		data["obj"] = "ACTION";
	}

	LOGDEBUG2(F("[ParameterizedSimpleAction]"), F("serializeJSON()"), F("OK: Serialized Action"), String(data.measureLength()), "", "");
}


template<class ActionType>
String NamedParameterizedSimpleAction<ActionType>::getTitle()
{
	return String(this->title + " " + (this->actionObject->*getForeignTitle)(this->parameter));
}

//All Types of Templates used:
template class SimpleAction<RCSocketController>;
template class ParameterizedSimpleAction<RCSocketController>;
template class NamedParameterizedSimpleAction<RCSocketController>;

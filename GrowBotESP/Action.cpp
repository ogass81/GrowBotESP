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
	this->group_title = group_title;
	this->antaObject = aObject;

	LOGDEBUG3(F("[Action]"), F("setAntagonist()"), F("OK: Set Antagonist Object in BASECLASS"), String(this->getTitle()), F("Antagonist"), String(this->antaObject->getTitle()));
}


template<class ActionType>
SimpleAction<ActionType>::SimpleAction(uint8_t id, String title, ActionType * actionObj, void(ActionType::*cFunct)(), bool visible)
{
	this->id = id;
	this->title = title;
	this->group_title = "";
	this->actionObject = actionObj;
	this->callback = cFunct;
	this->antaObject = NULL;
	this->visible = visible;

	LOGDEBUG3(F("[SimpleAction]"), F("SimpleAction()"), F("OK: Created Simple Action"), String(this->title), "", "");
}

template<class ActionType>
void SimpleAction<ActionType>::serializeJSON(JsonObject & data, Scope scope)
{
	if (scope == LIST || scope == DETAILS) {
		data["tit"] = title;
		data["grp"] = group_title;
		if (antaObject != NULL) data["anta"] = antaObject->getTitle();
		else data["anta"] = F("none");
		data["vis"] = visible;
	}

	if (scope == DETAILS) {
		data["id"] = id;
		data["obj"] = "ACTION";
	}

	LOGDEBUG2(F("[SimpleAction]"), F("serializeJSON()"), F("OK: Serialized members"), String(this->title), String(data.measureLength()), static_cast<int>(scope));
}

template<class ActionType>
void SimpleAction<ActionType>::execute()
{
	if (actionObject != NULL) {
		if(callback != NULL) {
			(actionObject->*callback)();
			LOGDEBUG(F("[SimpleAction]"), F("execute()"), F("OK: Executed Action"), getTitle(), "", "");
		}
		else LOGDEBUG(F("[SimpleAction]"), F("execute()"), F("ERROR: Callback missing"), getTitle(), "", "");
	}
	else LOGDEBUG(F("[SimpleAction]"), F("execute()"), F("ERROR: Action Object missing"), getTitle(), "", "");
}

template<class ActionType>
String SimpleAction<ActionType>::getTitle()
{
	return String(this->title);
}

template<class ActionType>
ParameterizedAction<ActionType>::ParameterizedAction(uint8_t id, String title, ActionType * actionObj, void(ActionType::*cFunct)(int), int par, bool visible)
{
	this->id = id;
	this->title = title;
	this->group_title = "";
	this->actionObject = actionObj;
	this->callback = cFunct;
	this->visible = visible;
	this->antaObject = NULL;
	this->parameter = par;

	LOGDEBUG3(F("[ParameterizedAction]"), F("SimpleAction()"), F("OK: Created ParameterizedAction"), String(this->title), F("Parameter"), String(par));
}

template<class ActionType>
void ParameterizedAction<ActionType>::serializeJSON(JsonObject & data, Scope scope)
{
	if (scope == LIST || scope == DETAILS) {
		data["tit"] = title;
		data["grp"] = group_title;
		if (antaObject != NULL) data["anta"] = antaObject->getTitle();
		else data["anta"] = F("none");
		data["vis"] = visible;
		data["par"] = parameter;
	}

	if (scope == DETAILS) {
		data["id"] = id;
		data["obj"] = "ACTION";
	}

	LOGDEBUG2(F("[ParameterizedAction]"), F("serializeJSON()"), F("OK: Serialized members"), F("Action"), String(this->title), String(data.measureLength()));
}

template<class ActionType>
void ParameterizedAction<ActionType>::execute()
{
	if (actionObject != NULL) {
		if (callback != NULL) {
			if (parameter >= 0) {
				(actionObject->*callback)(parameter);
				LOGDEBUG(F("[ParameterizedAction]"), F("execute()"), F("OK: Execute Action"), getTitle(), F("Parameter"), String(parameter));	
			}
			else {
				LOGDEBUG(F("[ParameterizedAction]"), F("execute()"), F("ERROR: Argument missing"), getTitle(), "", "");
			}
		}
		else LOGDEBUG(F("[ParameterizedAction]"), F("execute()"), F("ERROR: Callback missing"), getTitle(), "", "");
	}
	else LOGDEBUG(F("[ParameterizedAction]"), F("execute()"), F("ERROR: Action Object missing"), getTitle(), "", "");
}

template<class ActionType>
String ParameterizedAction<ActionType>::getTitle()
{
	return String(title);
}

template<class ActionType>
NamedParameterizedAction<ActionType>::NamedParameterizedAction(uint8_t id, String title, ActionType * actionObj, void(ActionType::* cFunct)(int), String(ActionType::* getTitle)(int), int par, bool visible) :
ParameterizedAction<ActionType>(id, title, actionObj, cFunct, par, visible)
{
	this->getForeignTitle = getTitle;

	LOGDEBUG3(F("[NamedParameterizedAction]"), F("SimpleAction()"), F("OK: Created NamedParameterizedAction"), String(this->title), F("Parameter"), String(par));
}

template<class ActionType>
void NamedParameterizedAction<ActionType>::serializeJSON(JsonObject & data, Scope scope)
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

	LOGDEBUG2(F("[NamedParameterizedAction]"), F("serializeJSON()"), F("OK: Serialized members"), F("Action"), String(this->title), String(data.measureLength()));
}


template<class ActionType>
String NamedParameterizedAction<ActionType>::getTitle()
{
	return String(this->title + " " + (this->actionObject->*getForeignTitle)(this->parameter));
}

//All Types of Templates used:
template class SimpleAction<RCSocketController>;
template class SimpleAction<TaskManager>;
template class ParameterizedAction<RCSocketController>;
template class ParameterizedAction<Trigger>;
template class NamedParameterizedAction<Trigger>;
template class NamedParameterizedAction<RCSocketController>;

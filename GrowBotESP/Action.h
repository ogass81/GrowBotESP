// Action.h

#ifndef _ACTION_h
#define _ACTION_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

//Helper
#include <ArduinoJson.h>
#include "LogEngine.h"

extern LogEngine logengine;

//Controller Object
#include "RCSocketController.h"

//Abstract Class for Actions
class Action {
public:
	String title;
	bool visible;
	
	//Pointer to Action that does the opposite
	String group_title;
	Action *antaObject = NULL;

	void setAntagonist(String group_title, Action *aObject);

	virtual void execute();
	//Serialize
	virtual void serializeJSON(uint8_t id, char* json, size_t maxSize, Scope scope);

	//UI Output
	virtual String getTitle();
};
// Wrapper for Simple Actions that include only one Callback Function
template <class ActionType>
class SimpleAction : public Action {
public:
	ActionType *actionObject = NULL;
	void (ActionType::*callback)();
	
	SimpleAction(String title, ActionType *actionObj, void (ActionType::*cFunct)(), bool visible = false);

	//Serialize
	void serializeJSON(uint8_t id, char* json, size_t maxSize, Scope scope);

	void execute();
	String getTitle();
};

template <class ActionType>
class ParameterizedSimpleAction : public Action {
public:
	ActionType *actionObject = NULL;
	void (ActionType::*callback)(int);
	int parameter;

	ParameterizedSimpleAction(String title, ActionType *actionObj, void (ActionType::*cFunct)(int), int par, bool visible = false);

	//Serialize
	void serializeJSON(uint8_t id, char* json, size_t maxSize, Scope scope);

	void execute();
	String getTitle();
};

template <class ActionType>
class NamedParameterizedSimpleAction : public ParameterizedSimpleAction<ActionType> {
public:
	String (ActionType::*getForeignTitle)(int);

	NamedParameterizedSimpleAction(String title, ActionType *actionObj, void (ActionType::*cFunct)(int), String (ActionType::*getTitle)(int), int par, bool visible = false);

	//Overwrite
	void serializeJSON(uint8_t id, char* json, size_t maxSize, Scope scope);
	String getTitle();
};

#endif



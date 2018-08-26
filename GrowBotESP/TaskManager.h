// TaskManager.h

#ifndef _TASKMANAGER_h
#define _TASKMANAGER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#include "Definitions.h"
#include "Action.h"
#include "ActionChain.h"
#include "LogEngine.h"
extern LogEngine logengine;

//Pre Declared for Compiler
class ActionChain;

class TaskManager {
private:
	uint8_t task_ptr = 0;
	Action *queue[TASK_QUEUE_LENGTH][TASK_PARALLEL_SEC];
	uint8_t getNextPosition(uint8_t delay);
	uint8_t getNextPositionFrom(uint8_t current_pos, uint8_t delay);
	uint8_t getOffSet(uint8_t duration);
	uint8_t getOffSet(ActionChain *actionchain);
public:
	TaskManager();
	void next();
	void prev();

	void addAction(Action *action, uint8_t duration);
	void addActions(ActionChain *actionchain);
	void execute();
};
#endif
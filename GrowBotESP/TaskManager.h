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
	int task_ptr = 0;
	Action *queue[TASK_QUEUE_LENGTH][TASK_PARALLEL_SEC];
	int getNextPosition(int delay);
	int getNextPositionFrom(int current_pos, int delay);
public:
	TaskManager();
	void next();
	void prev();

	void reset();

	void addActions(ActionChain *actionchain);
	void execute();
};
#endif
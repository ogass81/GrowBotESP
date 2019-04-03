// 
// 
// 

#include "TaskManager.h"

void TaskManager::next()
{
	if (task_ptr < (TASK_QUEUE_LENGTH - 1)) {
		task_ptr++;
	}
	else task_ptr = 0;
}

void TaskManager::prev()
{
	if (task_ptr > 0) {
		task_ptr--;
	}
	else task_ptr = (TASK_QUEUE_LENGTH - 1);
}

void TaskManager::reset()
{
	LOGDEBUG(F("[TaskManager]"), F("reset()"), F("OK: Reseting TaskManager"), "", "", "");

	for (int i = 0; i < TASK_QUEUE_LENGTH; i++) {
		for (int j = 0; j < TASK_PARALLEL_SEC; j++) {
			queue[i][j] = NULL;
		}
	}
	task_ptr = 0;
}

int TaskManager::getNextPosition(int delay)
{
	if (task_ptr + delay < TASK_QUEUE_LENGTH) {
		return int(task_ptr + delay);
	}
	else {
		return (delay - (TASK_QUEUE_LENGTH - task_ptr - 1));
	}
}

int TaskManager::getNextPositionFrom(int current_pos, int delay)
{
	if (current_pos + delay < TASK_QUEUE_LENGTH) {
		return int(current_pos + delay);
	}
	else {
		return (delay - (TASK_QUEUE_LENGTH - current_pos - 1));
	}
}

TaskManager::TaskManager()
{
	reset();
}


void TaskManager::addActions(ActionChain *actionchain)
{
	for (int slot = 0; slot < ACTIONCHAIN_LENGTH; slot++) {
		//Reset Values for each Action
		int current_ptr = task_ptr;
		int counter = 0;
		bool assigned = false;
		
		//Action assigned to slot
		if (actionchain->assignedAction[slot] != NULL) {
			//Simple Actions
			if (actionchain->actionPar[slot] == 0 || actionchain->assignedAction[slot]->antaObject == NULL) {
				LOGDEBUG1(F("[TaskManager]"), F("addActions()"), String(actionchain->getTitle()), F("INFO: Trying to add SIMPLE Action"), "Slot", String(slot));
				while (counter < TASK_QUEUE_LENGTH) {
					for (int i = 0; i < TASK_PARALLEL_SEC; i++) {
						LOGDEBUG1(F("[TaskManager]"), F("addActions()"), F("INFO: Checking Position"), String(counter), String(current_ptr), String(i));
						if (queue[current_ptr][i] == NULL) {
							queue[current_ptr][i] = actionchain->assignedAction[slot];
							assigned = true;
							LOGDEBUG1(F("[TaskManager]"), F("addActions()"), String(actionchain->getTitle()), F("INFO: Added SIMPLE Action to "), String(current_ptr), String(i));
							break;
						}
					}
					
					if (assigned == true) break;
					else {
						LOGDEBUG1(F("[TaskManager]"), F("addActions()"), F("INFO: No empty spot in row"), String(current_ptr), "", "");
						current_ptr = getNextPositionFrom(current_ptr, 1);
						counter++;
						LOGDEBUG1(F("[TaskManager]"), F("addActions()"), F("INFO: No empty spot in row"), String(current_ptr), String(counter), "");
					}
				}
			}
			//Complex Actions
			else {
				while (counter < TASK_QUEUE_LENGTH) {
					LOGDEBUG1(F("[TaskManager]"), F("addActions()"), String(actionchain->getTitle()), F("INFO: Trying to find empty spot in "), "row", String(current_ptr));
					for (int i = 0; i < TASK_PARALLEL_SEC; i++) {
						if (queue[current_ptr][i] == NULL) {

							int anta_ptr = getNextPosition(actionchain->actionPar[slot]);

							for (int j = 0; j < TASK_PARALLEL_SEC; j++) {
								if (queue[anta_ptr][j] == NULL) {
									queue[current_ptr][i]  = actionchain->assignedAction[slot];								
									queue[anta_ptr][j] = actionchain->assignedAction[slot]->antaObject;
									assigned = true;
									LOGDEBUG1(F("[TaskManager]"), F("addActions()"), String(actionchain->getTitle()), F("INFO: Added COMPLEX Action to "), String(current_ptr), String(anta_ptr));
									break;
								}
							}
						if (assigned == true) break;
						}
					}

					if (assigned == true) break;
					else {
						current_ptr = getNextPositionFrom(current_ptr, 1);
						counter++;
					}
				}
			}

			if (assigned == true) {
				LOGDEBUG1(F("[TaskManager]"), F("addActions()"), String(actionchain->getTitle()), F("INFO: Added Slot  to Task Queue"), String(current_ptr), "");
			}
			else {
				LOGDEBUG(F("[TaskManager]"), F("addActions()"), String(actionchain->getTitle()), F("ERROR: Could not add Slot "), String(slot), "");
			}
		}
		//No Action assigned to slot
		else {
			LOGDEBUG1(F("[TaskManager]"), F("addActions()"), F("INFO: no actions in chain"), String(actionchain->getTitle()), "", "");
		}
	}
}



void TaskManager::execute()
{
	//LOGDEBUG(F("[TaskManager]"), F("execute()"), F("OK: Taskpointer @"), String(task_ptr), F("of"), String(TASK_QUEUE_LENGTH));
	for (int i = 0; i < TASK_PARALLEL_SEC; i++) {
		if (queue[task_ptr][i] != NULL) {
			LOGDEBUG(F("[TaskManager]"), F("execute()"), F("OK: Executing task @"), String(task_ptr), String(queue[task_ptr][i]->getTitle()), "");
		
			queue[task_ptr][i]->execute();
			queue[task_ptr][i] = NULL;
		}
	}
	//Move Pointer forward
	next();
}
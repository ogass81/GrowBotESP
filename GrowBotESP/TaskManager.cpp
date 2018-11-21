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

uint8_t TaskManager::getNextPosition(uint8_t delay)
{
	if (task_ptr + delay < TASK_QUEUE_LENGTH) {
		return uint8_t(task_ptr + delay);
	}
	else {
		return (delay - (TASK_QUEUE_LENGTH - task_ptr - 1));
	}
}

uint8_t TaskManager::getNextPositionFrom(uint8_t current_pos, uint8_t delay)
{
	if (current_pos + delay < TASK_QUEUE_LENGTH) {
		return uint8_t(current_pos + delay);
	}
	else {
		return (delay - (TASK_QUEUE_LENGTH - current_pos - 1));
	}
}

TaskManager::TaskManager()
{
	for (uint8_t i = 0; i < TASK_QUEUE_LENGTH; i++) {
		for (uint8_t j = 0; j < TASK_PARALLEL_SEC; j++) {
			queue[i][j] = NULL;
		}
	}
	task_ptr = 0;
}


void TaskManager::addActions(ActionChain *actionchain)
{
	for (uint8_t slot = 0; slot < ACTIONCHAIN_LENGTH; slot++) {
		//Reset Values for each Action
		uint8_t current_ptr = task_ptr;
		uint8_t counter = 0;
		bool assigned = false;
		
		//Action assigned to slot
		if (actionchain->assignedAction[slot] != NULL) {
			//Simple Actions
			if (actionchain->actionPar[slot] == 0 || actionchain->assignedAction[slot]->antaObject == NULL) {
				LOGDEBUG3(F("[TaskManager]"), F("addActions()"), String(actionchain->getTitle()), F("INFO: Trying to add SIMPLE Action"), "Slot", String(slot));
				while (counter < TASK_QUEUE_LENGTH) {
					for (uint8_t i = 0; i < TASK_PARALLEL_SEC; i++) {
						if (queue[current_ptr][i] == NULL) {
							queue[current_ptr][i] = actionchain->assignedAction[slot];
							assigned = true;
							LOGDEBUG1(F("[TaskManager]"), F("addActions()"), String(actionchain->getTitle()), F("INFO: Added SIMPLE Action to "), String(current_ptr), String(i));
							break;
						}
					}
					
					if (assigned == true) break;
					else {
						current_ptr = getNextPosition(1);
						counter++;
					}
				}
			}
			//Complex Actions
			else {
				LOGDEBUG3(F("[TaskManager]"), F("addActions()"), String(actionchain->getTitle()), F("INFO: Trying to add COMPLEX Action"), "Slot", String(slot));

				while (counter < TASK_QUEUE_LENGTH) {
					for (uint8_t i = 0; i < TASK_PARALLEL_SEC; i++) {
						if (queue[current_ptr][i] == NULL) {

							uint8_t anta_ptr = getNextPosition(actionchain->actionPar[slot]);

							for (uint8_t j = 0; j < TASK_PARALLEL_SEC; j++) {
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
						current_ptr = getNextPosition(1);
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
	for (uint8_t i = 0; i < TASK_PARALLEL_SEC; i++) {
		if (queue[task_ptr][i] != NULL) {
			LOGDEBUG(F("[TaskManager]"), F("execute()"), F("OK: Executing task @"), String(task_ptr), String(queue[task_ptr][i]->getTitle()), "");
		
			queue[task_ptr][i]->execute();
			queue[task_ptr][i] = NULL;
		}
	}
	//Move Pointer forward
	next();
}
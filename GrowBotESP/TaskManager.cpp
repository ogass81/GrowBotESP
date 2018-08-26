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

uint8_t TaskManager::getOffSet(uint8_t duration)
{
	uint8_t current_ptr = task_ptr;
	bool empty_spot = false;
	uint8_t offset = 0;

	while (offset < TASK_QUEUE_LENGTH) {
		LOGDEBUG(F("[TaskManager]"), F("getOffSet()"), F("OK: Searching spot for timed Action with duration"), String(duration), String(current_ptr), String(offset));
	
		//Find Empty Spot for
		//Start
		for (uint8_t j = 0; j < TASK_PARALLEL_SEC; j++) {
			if (queue[current_ptr][j] == NULL) {
				current_ptr = getNextPositionFrom(current_ptr, duration);
				//End
				for (uint8_t k = 0; k < TASK_PARALLEL_SEC; k++) {
					if (queue[current_ptr][k] == NULL) {
						empty_spot = true;
						current_ptr = getNextPositionFrom(current_ptr, 0); //Allow Parallel Tasks -> 0
						break;
					}
					else empty_spot = false;
				}
				break;
			}
			else empty_spot = false;
		}
	
		if (empty_spot == false) { //Last Task didnt find spot - bad -> start over with different offset
			offset++;
			current_ptr = getNextPositionFrom(task_ptr, offset);
			LOGDEBUG2(F("[TaskManager]"), F("getOffSet()"), F("INFO: No spot found for Action with duration"), String(duration), String("Raising Pointer and Offset"), String(offset));
		}
		
		if (empty_spot == true) {
			LOGDEBUG2(F("[TaskManager]"), F("getOffSet()"), F("Ok: Spot found for Action with duration"), String(duration), String(current_ptr), String(offset));
			break; //Last Task found spot - good -> break loop
		}
	}

	return offset;
}

uint8_t TaskManager::getOffSet(ActionChain *actionchain)
{
	uint8_t current_ptr = task_ptr;
	bool empty_spot = false;
	uint8_t offset = 0;

	while (offset < TASK_QUEUE_LENGTH) {
		LOGDEBUG(F("[TaskManager]"), F("getOffSet()"), F("OK: Searching spot for Actionchain"), String(actionchain->getTitle()), String(current_ptr), String(offset));
	
		for (uint8_t i = 0; i < ACTIONCHAIN_LENGTH; i++) { //Iterate all Tasks in Chain
			if (actionchain->assignedAction[i] != NULL) {
				if (actionchain->actionPar[i] == 0) {  //Simple Action - only Start, no End
					 //Empty Spot for Start of Action
					for (uint8_t j = 0; j < TASK_PARALLEL_SEC; j++) {
						if (queue[current_ptr][j] == NULL) {
							empty_spot = true;
							current_ptr = getNextPositionFrom(current_ptr, 0); //Allow Parallel Tasks -> 0
							break;
						}
						else empty_spot = false;
					}
				}
				else {  //Complex Action - need two spots for Start and End
						//Find Empty Spot for
						//Start
					for (uint8_t j = 0; j < TASK_PARALLEL_SEC; j++) {
						if (queue[current_ptr][j] == NULL) {
							current_ptr = getNextPositionFrom(current_ptr, actionchain->actionPar[i]);
							//End
							for (uint8_t k = 0; k < TASK_PARALLEL_SEC; k++) {
								if (queue[current_ptr][k] == NULL) {
									empty_spot = true;
									current_ptr = getNextPositionFrom(current_ptr, 0); //Allow Parallel Tasks -> 0
									break;
								}
								else empty_spot = false;
							}
							break;
						}
						else empty_spot = false;
					}
				}
			}
			else break; //No Action No Effort -> Break

			if (empty_spot == false) { //Last Task didnt find spot - bad -> start over with different offset
				offset++;
				current_ptr = getNextPositionFrom(task_ptr, offset);
				LOGDEBUG(F("[TaskManager]"), F("getOffSet()"), F("INFO: No spot found for Action"), String(i), String("Raising Pointer and Offset"), String(offset));
				break;
			}
		}
		if (empty_spot == true) {
			LOGDEBUG(F("[TaskManager]"), F("getOffSet()"), F("Ok: Spot found for Actionchain"), String(actionchain->getTitle()), String(current_ptr), String(offset));
			break; //Last Task found spot - good -> break loop
		}
	}

	return offset;
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

void TaskManager::addAction(Action * action, uint8_t duration)
{
	uint8_t current_ptr = task_ptr;
	uint8_t offset = 0;

	offset = getOffSet(duration);
	if (offset < TASK_QUEUE_LENGTH) {
		current_ptr = getNextPositionFrom(task_ptr, offset);

		for (uint8_t j = 0; j < TASK_PARALLEL_SEC; j++) {
			if (queue[current_ptr][j] == NULL) {
				uint8_t start_ptr, start_task;
				queue[current_ptr][j] = action;
				LOGMSG(F("[TaskManager]"), F("OK: Added action to queue @"), String(current_ptr), String(action->getTitle()), "");
				
				/*
				String keys[] = { "Position", "Action"};
				String values[] = { (String)current_ptr, action->getTitle()};
				logengine.addLogEntry(ACTION, "TaskManager", "Added action to queue", keys, values, 2);
				*/

				//If adding of antagonist task fails -> needed to remove previously added action
				start_ptr = current_ptr;
				start_task = j;

				current_ptr = getNextPositionFrom(current_ptr, duration);
				//End
				for (uint8_t k = 0; k < TASK_PARALLEL_SEC; k++) {
					if (queue[current_ptr][k] == NULL) {
						if (action->antaObject != NULL) {
							queue[current_ptr][k] = action->antaObject;
							LOGMSG(F("[TaskManager]"), F("OK: Added action to queue @"), String(current_ptr), String(action->antaObject->getTitle()), "");

							/*
							String keys[] = { "Position", "Action" };
							String values[] = { (String)current_ptr, action->antaObject->getTitle() };
							logengine.addLogEntry(ACTION, "TaskManager", "Added action to queue", keys, values, 2);
						*/
						}
						else {
							queue[start_ptr][start_task] = NULL;
							LOGMSG(F("[TaskManager]"), F("ERROR: Antagonist Object missing"), F("Removed Start Action from"), start_ptr, start_task);

							
							String keys[] = { "Position", "Start Task" };
							String values[] = { (String)start_ptr, (String)start_task };
							logengine.addLogEntry(ACTION, "TaskManager", "Antagonist Object missing", keys, values, 2);
					
						}
						current_ptr = getNextPositionFrom(current_ptr, 0); //Allow Parallel Tasks -> 0
						break;
					}
				}
				break;
			}
		}
	}
	else {
		LOGMSG(F("[TaskManager]"), F("ERROR: Could not find spot for Action"), String(action->getTitle()), F("Duration"), String(duration));

		String keys[] = { "Action", "Duration" };
		String values[] = { action->getTitle(), String(duration) };
		logengine.addLogEntry(ERROR, "TaskManager", "Failed to add Action to Task Queue", keys, values, 2);
	}
}

void TaskManager::addActions(ActionChain *actionchain)
{
	uint8_t current_ptr = task_ptr;
	uint8_t offset = 0;

	offset = getOffSet(actionchain);

	if (offset < TASK_QUEUE_LENGTH) {
		current_ptr = getNextPositionFrom(task_ptr, offset);
		
		for (uint8_t i = 0; i < ACTIONCHAIN_LENGTH; i++) { //Iterate all Tasks in Chain
			if (actionchain->assignedAction[i] != NULL) {
					if (actionchain->actionPar[i] == 0) {  //Simple Action - only Start, no End
														 //Empty Spot for Start of Action
					for (uint8_t j = 0; j < TASK_PARALLEL_SEC; j++) {
						if (queue[current_ptr][j] == NULL) {
							queue[current_ptr][j] = actionchain->assignedAction[i];
							current_ptr = getNextPositionFrom(current_ptr, 0); //Allow Parallel Tasks -> 0
														
							LOGMSG(F("[TaskManager]"), F("OK: Added action to queue @"), String(current_ptr), String(actionchain->assignedAction[i]->getTitle()), "");

							/*
							String keys[] = { "Position", "Action" };
							String values[] = { (String)current_ptr, actionchain->assignedAction[i]->getTitle() };
							logengine.addLogEntry(ACTION, "TaskManager", "Added action to queue", keys, values, 2);
							*/
							break;

						}
					}
				}
				else {  //Complex Action - need two spots for Start and End
						//Find Empty Spot for
						//Start
					for (uint8_t j = 0; j < TASK_PARALLEL_SEC; j++) {
						if (queue[current_ptr][j] == NULL) {
							uint8_t start_ptr, start_task;
							queue[current_ptr][j] = actionchain->assignedAction[i];
							LOGMSG(F("[TaskManager]"), F("OK: Added action to queue @"), String(current_ptr), String(actionchain->assignedAction[i]->getTitle()), "");

							String keys[] = { "Position", "Action" };
							String values[] = { (String)current_ptr, actionchain->assignedAction[i]->getTitle() };
							logengine.addLogEntry(ACTION, "TaskManager", "Added action to queue", keys, values, 2);
							
							//If adding of antagonist task fails -> needed to remove previously added action
							start_ptr = current_ptr;
							start_task = j;
							
							current_ptr = getNextPositionFrom(current_ptr, actionchain->actionPar[i]);
							//End
							for (uint8_t k = 0; k < TASK_PARALLEL_SEC; k++) {
								if (queue[current_ptr][k] == NULL) {
									if (actionchain->assignedAction[i]->antaObject != NULL) {
										queue[current_ptr][k] = actionchain->assignedAction[i]->antaObject;
										LOGMSG(F("[TaskManager]"), F("OK: Added action to queue @"), String(current_ptr), String(actionchain->assignedAction[i]->antaObject->getTitle()), "");

										/*
										String keys[] = { "Position", "Action" };
										String values[] = { (String)current_ptr, actionchain->assignedAction[i]->antaObject->getTitle() };
										logengine.addLogEntry(ACTION, "TaskManager", "Added action to queue", keys, values, 2);
										*/
									}
									else {
										queue[start_ptr][start_task] = NULL;
										LOGMSG(F("[TaskManager]"), F("ERROR: Antagonist Object missing."), F("Removed Start Action from"), start_ptr, start_task);

										
										String keys[] = { "Position", "Start Task" };
										String values[] = { (String)start_ptr, (String)start_task };
										logengine.addLogEntry(ERROR, "TaskManager", "Antagonist Object missing", keys, values, 2);
										
									}
									current_ptr = getNextPositionFrom(current_ptr, 0); //Allow Parallel Tasks -> 0
									break;
								}
							}
							break;
						}
					}
				}
			}
			else break; //No Action No Effort -> Break
		} 
	}
	else {
		LOGMSG(F("[TaskManager]"), F("ERROR: Could not find spot for all tasks of Actionchain."), String(actionchain->getTitle()), F("Current # of parallel tasks"), String(TASK_PARALLEL_SEC));

		String keys[] = { "Actionchain", "Duration" };
		String values[] = { actionchain->getTitle(), String(TASK_PARALLEL_SEC) };
		logengine.addLogEntry(ERROR, "TaskManager", "Could not find spots for all tasks of Actionchain", keys, values, 2);
	}
}

void TaskManager::execute()
{
	//LOGDEBUG(F("[TaskManager]"), F("execute()"), F("OK: Taskpointer @"), String(task_ptr), F("of"), String(TASK_QUEUE_LENGTH));
	for (uint8_t i = 0; i < TASK_PARALLEL_SEC; i++) {
		if (queue[task_ptr][i] != NULL) {
			LOGMSG(F("[TaskManager]"), F("OK: Executing task @"), String(task_ptr), String(queue[task_ptr][i]->getTitle()), "");
		
			queue[task_ptr][i]->execute();
			queue[task_ptr][i] = NULL;
		}
	}
	//Move Pointer forward
	next();
}
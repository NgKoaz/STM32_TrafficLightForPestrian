/*
 * scheduler.c
 *
 *  Created on: Oct 29, 2023
 *      Author: HP
 */

#include "scheduler.h"
#include "stdlib.h"

sTask* head_task;

/*
 * While loop can be slow or hang a while, then it cannot execute the next task
 * that task has Delay = 0.
 * So we need time_skip to make sure it run correct number of times.
 */
uint16_t time_skip;

// Number of tasks are ready
uint16_t count_task;

/*
 * This id will be assign for task, we increase 1 when assign it for TaskID
 * to make sure it's always available
 */
uint16_t next_id;

/*
 * For display on Terminal
 * when sometask run at that time it's will print the `timestamp` and `taskID` just ran.
 */
uint16_t TaskIdJustRun = 0;

/*
 * For report error
 */
uint8_t errorCode;

void SCH_Init(void){
	head_task 	= NULL;
	time_skip 	= 0;
	count_task 	= 0;
	next_id 	= 1;
	errorCode	= 0;
}

void SCH_Update(void){
	if (head_task == NULL) {
		/*
		 * This is for situation, you just have one task run.
		 * When task ran completely, it will dequeue, but at this time
		 * SCH_Update can be run, and if we not add 1 for time_skip
		 * instead of do nothing, your task will run wrong time.
		 */
		time_skip = (count_task > 0) ? time_skip + 1 : 0;
		return;
	}
	time_skip++;
	if (head_task->Delay > 0){
		int temp = head_task->Delay - time_skip;
		if (temp >= 0) {
			head_task->Delay = temp;
			time_skip = 0;
		} else {
			head_task->Delay = 0;
			time_skip = 0 - temp;
		}
	}
}

sTask* Create_Task(void (*pFunc)(void), uint32_t Delay, uint32_t Period){
	if (count_task > SCH_MAX_TASKS){
		errorCode = ERROR_SCH_TOO_MANY_TASKS;
		return NULL;
	}
	uint16_t newId = next_id++;

	Delay  /= TIMER_CYCLE;
	Period /= TIMER_CYCLE;

	sTask* newTask = malloc(sizeof(sTask));

	newTask->pFunc 		= pFunc;
	newTask->Delay 		= Delay + time_skip;
	newTask->Period	    = Period;
	newTask->TaskID		= newId;
	newTask->Next 		= NULL;

	return newTask;
}

/*
 * Enqueue and modify Delay
 * Example: A->Delay = 1000, B->Delay = 2000, C->Delay = 3000
 * When we completely add all of them in queue, the queue will be
 * Element 1: A->Delay = 1000, Element 2: B->Delay = 1000, C->Delay = 1000
 */
sTask* Enqueue_Task(sTask* newTask){
	//First task in queue.
	if (head_task == NULL){
		head_task = newTask;
		return newTask;
	}

	//Task will add at middle of queue.
	sTask *ini = head_task;
	sTask *pre = NULL;
	uint32_t sum = 0;
	while (ini != NULL){
		if (sum + ini->Delay > newTask->Delay){
			if (ini == head_task){
				newTask->Next = head_task;
				head_task 	= newTask;
				ini->Delay -= newTask->Delay;
			} else {
				newTask->Next 	= ini;
				pre->Next 		= newTask;
				newTask->Delay -= sum;
				ini->Delay     -= newTask->Delay;
			}
			return newTask;
		}
		sum 	+= ini->Delay;
		pre		 = ini;
		ini		 = ini->Next;
	}

	//Task will add of tail
	if (ini == NULL){
		pre->Next 		= newTask;
		newTask->Delay -= sum;
	}

	return newTask;
}

sTask* SCH_Add_Task(void (*pFunc)(void), uint32_t Delay, uint32_t Period){
	sTask* newTask = Create_Task(pFunc, Delay, Period);
	if (newTask == NULL){
		return NULL;
	}
	return Enqueue_Task(newTask);
}


/*
 * If no task run => return 0
 * task run => return 1
 */
uint8_t SCH_Dispatch_Tasks(void){
	if (head_task == NULL || head_task->Delay > 0) return 0;

	// Remove head task out of queue (dequeue, not delete)
	// and config delay then enqueue it again.
	sTask* runningTask = head_task;
	head_task = head_task->Next;

	runningTask->Next 	= NULL;
	runningTask->Delay 	= runningTask->Period;

	// Run task
	runningTask->pFunc();
	// Save the task just ran
	TaskIdJustRun = runningTask->TaskID;

	// Add again if a task has period value isn't equal to 0.
	if (runningTask->Period != 0) Enqueue_Task(runningTask);
	else free(runningTask);

	return 1;
}

uint8_t SCH_Delete_Task(sTask* deleted_task){
	if (deleted_task == NULL){
		errorCode = ERROR_SCH_DELETE_NULL_TASK;
		return 0;
	}
	if (deleted_task == head_task){
		count_task--;
		head_task = head_task->Next;
		deleted_task->Next->Delay += deleted_task->Delay;
		free(deleted_task);
		return 1;
	}

	// Find previous of deleted task
	sTask* pre = NULL;
	sTask* ini = head_task;
	while (ini != deleted_task && ini != NULL){
		pre = ini;
		ini = ini->Next;
	}

	if (ini == NULL){
		// Task not found
		errorCode = ERROR_SCH_DELETE_NULL_TASK;
		return 0;
	}

	if (deleted_task->Next != NULL){
		deleted_task->Next->Delay += deleted_task->Delay;
	}
	if (pre != NULL){
		pre->Next = deleted_task->Next;
	}
	free(deleted_task);
	return 1;
}

void SCH_Sleep(void){
	//Still task need to run
	if (head_task->Delay == 0) return;
	//No task to run at this time, then go sleep
	HAL_SuspendTick();
	HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
	HAL_ResumeTick();
}

uint8_t SCH_Report_Status(void){
#ifdef SCH_REPORT_ERRORS
	uint8_t tempErrorCode = errorCode;
	errorCode = 0;
	return tempErrorCode;
#endif
}

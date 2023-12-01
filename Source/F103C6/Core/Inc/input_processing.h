/*
 * input_processing.h
 *
 *  Created on: Oct 8, 2023
 *      Author: HP
 */

#ifndef INC_INPUT_PROCESSING_H_
#define INC_INPUT_PROCESSING_H_

#include "main.h"
#include "input_reading.h"
#include "global.h"
#include "string.h"
#include "stdio.h"

//After we hold modify button more than 1s,
//it will increase after this milisecond.
#define INCREASING_PERIOD 	100 / CYCLE

typedef enum{
	RELEASED,
	PRESSED,
	PRESSED_MORE_THAN_ONE_SECOND
} ButtonState;

typedef enum {
	NORMAL_MODE = 1,
	MODIFY_DURATION_RED_MODE = 2,
	MODIFY_DURATION_AMBER_MODE = 3,
	MODIFY_DURATION_GREEN_MODE = 4
} RunState;

typedef enum {
	RED = 3,
	AMBER = 5,
	GREEN = 6
} LEDState;

void inputProcessingInit(UART_HandleTypeDef* huart);

void handleSetValueButton(void);
void handleModifyButton(void);
void handleSelectModeButton(void);
void handlePedestrianButton(void);

//change display
void runStateFSM(void);


#endif /* INC_INPUT_PROCESSING_H_ */

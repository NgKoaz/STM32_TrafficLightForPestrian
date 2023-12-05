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
#include "traffic_light.h"
#include "i2c-lcd.h"

//After we hold modify button more than 1s,
//it will increase after this milisecond.
#define INCREASING_PERIOD 	100 / CYCLE



void inputProcessingInit(UART_HandleTypeDef* huart);

void displayPLED(void);

void handleSetValueButton(void);
void handleModifyButton(void);
void handleSelectModeButton(void);
void handlePedestrianButton(void);

//change display
void runStateFSM(void);


#endif /* INC_INPUT_PROCESSING_H_ */

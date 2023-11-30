/*
 * input_reading.h
 *
 *  Created on: Oct 8, 2023
 *      Author: HP
 */

#ifndef INC_INPUT_READING_H_
#define INC_INPUT_READING_H_

#include "main.h"
#include "global.h"

#define BUTTON_PRESSED 	GPIO_PIN_RESET
#define BUTTON_RELEASED GPIO_PIN_SET

//Number of buttons
#define NO_BUTTON 		4

//Index for each button
#define SEL_BTN 		0
#define MOD_BTN 		1
#define SET_BTN 		2
#define P_BTN			3

void 	inputReadingInit(void);
void 	buttonReading(const short index);
uint8_t isButtonPressed(const short index);
uint8_t isButtonPressedOneSec(const short index);

#endif /* INC_INPUT_READING_H_ */

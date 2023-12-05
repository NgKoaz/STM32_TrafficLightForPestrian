/*
 * input_reading.c
 *
 *  Created on: Oct 8, 2023
 *      Author: HP
 */


#include "input_reading.h"

//Set Port and Pin for button
GPIO_TypeDef* BTN_GPIO[NO_BUTTON] = {
		SEL_BTN_GPIO_Port,
		MOD_BTN_GPIO_Port,
		SET_BTN_GPIO_Port,
		P_BTN_GPIO_Port
};
const short BTN_Pin[NO_BUTTON] = {
		SEL_BTN_Pin,
		MOD_BTN_Pin,
		SET_BTN_Pin,
		P_BTN_Pin
};

//Current state of button
GPIO_PinState filtered[NO_BUTTON];

//Two debounce buffer.
GPIO_PinState buffer0[NO_BUTTON];
GPIO_PinState buffer1[NO_BUTTON];

//These variable use for handling pressed more than one second.
uint8_t flagOneSec[NO_BUTTON];
short counterOneSec[NO_BUTTON];

void inputReadingInit(void){
	for(short i = 0; i < NO_BUTTON; i++){
		filtered[i] = buffer0[i] = buffer1[i] = BUTTON_RELEASED;
		flagOneSec[i] = 0;
		counterOneSec[i] = ONE_SECOND;
	}
}

void buttonReading(const short index){
	//Save 2 recently states
	buffer1[index] = buffer0[index];
	buffer0[index] = HAL_GPIO_ReadPin(BTN_GPIO[index], BTN_Pin[index]);

	//If all saved states equal -> that's a good reading (filtered)
	if(buffer1[index] == buffer0[index]){
		filtered[index] = buffer0[index];
		//If button_pressed -> counter will work, if not -> just reset counter.
		if (filtered[index] == BUTTON_PRESSED){
			counterOneSec[index]--;
			if (counterOneSec[index] <= 0){
				flagOneSec[index] = 1;
			}
		} else {
			counterOneSec[index] = ONE_SECOND;
			flagOneSec[index] = 0;
		}
	}
}

uint8_t isButtonPressed(const short index){
	return (filtered[index] == BUTTON_PRESSED);
}

uint8_t isButtonPressedOneSec(const short index){
	return flagOneSec[index];
}






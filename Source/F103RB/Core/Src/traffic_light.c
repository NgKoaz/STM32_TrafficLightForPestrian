/*
 * traffic_light.c
 *
 *  Created on: Oct 12, 2023
 *      Author: HP
 */
#include "traffic_light.h"


GPIO_TypeDef* TL_GPIO[DIMENSION * 2] = {
		TL1_A_GPIO_Port, TL1_B_GPIO_Port,
		TL2_A_GPIO_Port, TL2_B_GPIO_Port
};

const short TL_Pin[DIMENSION * 2] = {
		TL1_A_Pin, TL1_B_Pin,
		TL2_A_Pin, TL2_B_Pin
};

void resetAllLED(void){
	HAL_GPIO_WritePin(TL_GPIO[0], TL_Pin[0], RESET);
	HAL_GPIO_WritePin(TL_GPIO[1], TL_Pin[1], RESET);
	HAL_GPIO_WritePin(TL_GPIO[2], TL_Pin[2], RESET);
	HAL_GPIO_WritePin(TL_GPIO[3], TL_Pin[3], RESET);
}

void WritePinLED(const short index, uint8_t state){
	HAL_GPIO_WritePin(TL_GPIO[index * 2 + 1], TL_Pin[index * 2 + 1], state & 1);
	state >>= 1;
	HAL_GPIO_WritePin(TL_GPIO[index * 2], TL_Pin[index * 2], state & 1);
}

//Depend on `state` parameter. If RED, blinking RED, so on.
void TogglePinLED(uint8_t state){
	switch(state){
	case RED:
		HAL_GPIO_TogglePin(TL1_A_GPIO_Port, TL1_A_Pin);
		HAL_GPIO_TogglePin(TL2_A_GPIO_Port, TL2_A_Pin);
		break;
	case AMBER:
		HAL_GPIO_TogglePin(TL1_A_GPIO_Port, TL1_A_Pin);
		HAL_GPIO_TogglePin(TL1_B_GPIO_Port, TL1_B_Pin);

		HAL_GPIO_TogglePin(TL2_A_GPIO_Port, TL2_A_Pin);
		HAL_GPIO_TogglePin(TL2_B_GPIO_Port, TL2_B_Pin);
		break;
	case GREEN:
		HAL_GPIO_TogglePin(TL1_B_GPIO_Port, TL1_B_Pin);
		HAL_GPIO_TogglePin(TL2_B_GPIO_Port, TL2_B_Pin);
 		break;
	}
}

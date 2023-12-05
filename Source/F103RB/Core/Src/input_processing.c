/*
 * input_processing.c
 *
 *  Created on: Oct 8, 2023
 *      Author: HP
 */

#include "input_processing.h"


ButtonState buttonState[NO_BUTTON];
RunState runState;
LEDState ledState[DIMENSION];
uint16_t light_counter[DIMENSION];

PedestrianState pState;

uint8_t durationRed;
uint8_t durationAmber;
uint8_t durationGreen;

uint8_t adjust_duRed;
uint8_t adjust_duAmber;
uint8_t adjust_duGreen;

char message[16];

//For blinking led when we switch in modify mode.
int blinking_counter;
//This counter for modify button after amount of time it increase `adjusting variable`.
int increasing_counter;

short buzzer_counter;

short pedestrian_led_counter = ONE_SECOND / 4;

void buzzerNoBeep(){
	HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, SET);
}

void resetTrafficLight(void){
	pState = SLEEP;
	ledState[VERTICAL] = GREEN;
	ledState[HORIZONTAL] = RED;
	light_counter[VERTICAL]   = durationGreen * ONE_SECOND;
	light_counter[HORIZONTAL] = durationRed * ONE_SECOND;
	buzzerNoBeep();
}

void toggleLED(void){
	HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
}

void displayPLED(void){
	switch (pState){
	case SLEEP:
		HAL_GPIO_WritePin(PLED_RED_GPIO_Port, PLED_RED_Pin, 0);
		HAL_GPIO_WritePin(PLED_GREEN_GPIO_Port, PLED_GREEN_Pin, 1);
		break;
	case WAIT:
		pedestrian_led_counter--;
		if (pedestrian_led_counter <= 0){
			pedestrian_led_counter = ONE_SECOND / 4;
			HAL_GPIO_TogglePin(PLED_RED_GPIO_Port, PLED_RED_Pin);
		}
		HAL_GPIO_WritePin(PLED_GREEN_GPIO_Port, PLED_GREEN_Pin, 1);
		break;
	case ACROSS:
		HAL_GPIO_WritePin(PLED_RED_GPIO_Port, PLED_RED_Pin, 1);
		HAL_GPIO_WritePin(PLED_GREEN_GPIO_Port, PLED_GREEN_Pin, 0);
		break;
	}
}

void inputProcessingInit(UART_HandleTypeDef* huart)
{
	buttonState[SET_BTN] = RELEASED;
	buttonState[MOD_BTN] = RELEASED;
	buttonState[SEL_BTN] = RELEASED;
	buttonState[P_BTN] 	 = RELEASED;
	blinking_counter = 1;
	increasing_counter = INCREASING_PERIOD;

	durationRed = DURATION_RED_DEFAULT;
	durationAmber = DURATION_AMBER_DEFAULT;
	durationGreen = DURATION_GREEN_DEFAULT;

	light_counter[VERTICAL]   = durationGreen * ONE_SECOND;
	light_counter[HORIZONTAL] = durationRed * ONE_SECOND;

	runState = NORMAL_MODE;
	resetTrafficLight();

}

void increaseOne(uint8_t* duration){
	(*duration) = (*duration) + 1 > 99 ? 0 : (*duration) + 1;
}

void modifyingValue(void){
	switch(runState){
	case NORMAL_MODE:
		break;
	case MODIFY_DURATION_RED_MODE:
		increaseOne(&adjust_duRed);

		lcd_clear();
		lcd_put_cur(0, 0);
		sprintf(message, "CUR_VALUE: %ds", durationRed);
		lcd_send_string(message);
		lcd_put_cur(1, 0);
		sprintf(message, "MOD_RED: %ds", adjust_duRed);
		lcd_send_string(message);

		break;
	case MODIFY_DURATION_AMBER_MODE:
		increaseOne(&adjust_duAmber);

		lcd_clear();
		lcd_put_cur(0, 0);
		sprintf(message, "CUR_VALUE: %ds", durationAmber);
		lcd_send_string(message);
		lcd_put_cur(1, 0);
		sprintf(message, "MOD_AMBER: %ds", adjust_duAmber);
		lcd_send_string(message);

		break;
	case MODIFY_DURATION_GREEN_MODE:
		increaseOne(&adjust_duGreen);

		lcd_clear();
		lcd_put_cur(0, 0);
		sprintf(message, "CUR_VALUE: %ds", durationGreen);
		lcd_send_string(message);
		lcd_put_cur(1, 0);
		sprintf(message, "MOD_GREEN: %ds", adjust_duGreen);
		lcd_send_string(message);
		break;
	}
}
void changingMode(void){
	//Turn off all LEDs.
	resetAllLED();

	//Changing state and initial new value for new mode.
	switch(runState){
	case NORMAL_MODE:
		runState = MODIFY_DURATION_RED_MODE;
		blinking_counter = HALF_SECOND;
		adjust_duRed = durationRed;
		resetTrafficLight();

		lcd_clear();
		lcd_put_cur(0, 0);
		sprintf(message, "CUR_VALUE: %ds", durationRed);
		lcd_send_string(message);
		lcd_put_cur(1, 0);
		sprintf(message, "MOD_RED: %ds", adjust_duRed);
		lcd_send_string(message);

		break;
	case MODIFY_DURATION_RED_MODE:
		runState = MODIFY_DURATION_AMBER_MODE;
		blinking_counter = HALF_SECOND;
		adjust_duAmber = durationAmber;
		resetTrafficLight();

		lcd_clear();
		lcd_put_cur(0, 0);
		sprintf(message, "CUR_VALUE: %ds", durationAmber);
		lcd_send_string(message);
		lcd_put_cur(1, 0);
		sprintf(message, "MOD_AMBER: %ds", adjust_duAmber);
		lcd_send_string(message);

		break;
	case MODIFY_DURATION_AMBER_MODE:
		runState = MODIFY_DURATION_GREEN_MODE;
		blinking_counter = HALF_SECOND;
		adjust_duGreen = durationGreen;
		resetTrafficLight();

		lcd_clear();
		lcd_put_cur(0, 0);
		sprintf(message, "CUR_VALUE: %ds", durationGreen);
		lcd_send_string(message);
		lcd_put_cur(1, 0);
		sprintf(message, "MOD_GREEN: %ds", adjust_duGreen);
		lcd_send_string(message);

		break;
	case MODIFY_DURATION_GREEN_MODE:
		runState = NORMAL_MODE;
		resetTrafficLight();
		break;
	}
}
void setValue(void){
	switch(runState){
	case NORMAL_MODE:
		break;
	case MODIFY_DURATION_RED_MODE:
		durationRed = adjust_duRed;

		lcd_clear();
		lcd_put_cur(0, 0);
		sprintf(message, "CUR_VALUE: %ds", durationRed);
		lcd_send_string(message);
		lcd_put_cur(1, 0);
		sprintf(message, "MOD_RED: %ds", adjust_duRed);
		lcd_send_string(message);

		break;
	case MODIFY_DURATION_AMBER_MODE:
		durationAmber = adjust_duAmber;

		lcd_clear();
		lcd_put_cur(0, 0);
		sprintf(message, "CUR_VALUE: %ds", durationAmber);
		lcd_send_string(message);
		lcd_put_cur(1, 0);
		sprintf(message, "MOD_AMBER: %ds", adjust_duAmber);
		lcd_send_string(message);

		break;
	case MODIFY_DURATION_GREEN_MODE:
		durationGreen = adjust_duGreen;

		lcd_clear();
		lcd_put_cur(0, 0);
		sprintf(message, "CUR_VALUE: %ds", durationGreen);
		lcd_send_string(message);
		lcd_put_cur(1, 0);
		sprintf(message, "MOD_GREEN: %ds", adjust_duGreen);
		lcd_send_string(message);

		break;
	}
}
void handlePedestrianPressedEvent(){
	if (runState != NORMAL_MODE) return;
	switch(pState){
	case SLEEP:
		if (ledState[VERTICAL] == GREEN || ledState[VERTICAL] == AMBER
				|| (ledState[VERTICAL] == RED && light_counter[VERTICAL] < 8 * ONE_SECOND)
		) {
			pState = WAIT;
		} else {
			pState = ACROSS;
		}
		break;
	case WAIT:
		break;
	case ACROSS:
		break;
	default:
		pState = SLEEP;
	}
}

//This is abstract function. Use for those function below.
void inputProcessingFSM(void (*processing) (void), const short index){
	switch(buttonState[index]){
	case RELEASED:
		if (isButtonPressed(index)){
			buttonState[index] = PRESSED;
			(*processing)();
		}
		break;
	case PRESSED:
		if (!isButtonPressed(index)){
			buttonState[index] = RELEASED;
		} else if (isButtonPressedOneSec(index)){
			buttonState[index] = PRESSED_MORE_THAN_ONE_SECOND;
		}
		break;
	case PRESSED_MORE_THAN_ONE_SECOND:
		if (!isButtonPressedOneSec(index)){
			buttonState[index] = RELEASED;
		}
		break;
	}
}
void handleSetValueButton(void){
	buttonReading(SET_BTN);
	inputProcessingFSM(setValue, SET_BTN);
}
void handleModifyButton(void){
	buttonReading(MOD_BTN);
	inputProcessingFSM(modifyingValue, MOD_BTN);
	//Handle when button hold more than one second
	//increase `adjusting value` after INCREASING_PERIOD ms
	if (buttonState[MOD_BTN] == PRESSED_MORE_THAN_ONE_SECOND) {
		increasing_counter--;
		if (increasing_counter <= 0) {
			increasing_counter = INCREASING_PERIOD;
			modifyingValue();
		}
	} else {
		increasing_counter = INCREASING_PERIOD;
	}
}
void handleSelectModeButton(void){
	buttonReading(SEL_BTN);
	inputProcessingFSM(changingMode, SEL_BTN);
}

void handlePedestrianButton(void){
	buttonReading(P_BTN);
	//.inputProcessingFSM(toggleLED, P_BTN);
	inputProcessingFSM(handlePedestrianPressedEvent, P_BTN);
}


uint16_t setCounterForBuzzer(){
	uint8_t counter = light_counter[VERTICAL] / ONE_SECOND;
	if (counter <= 3){
		return ONE_SECOND / 20;
	}
	else if (counter <= 6){
		return ONE_SECOND / 10;
	}
	else if (counter <= 12){
		return ONE_SECOND / 5;
	}
	else {
		return ONE_SECOND / 2;
	}

}

void displayingDuration(uint8_t index){
	if (light_counter[index] % ONE_SECOND != 0 || light_counter[index] == 0)
		return;

	char* orientation;
	if (index == VERTICAL) orientation = "V";
	else orientation = "H";

	char* led_state_name;
	switch(ledState[index]){
	case RED:
		led_state_name = "RED";
		break;
	case AMBER:
		led_state_name = "AMBER";
		break;
	case GREEN:
		led_state_name = "GREEN";
		break;
	}

	uint16_t numberDisplay = (light_counter[index] - 1) / ONE_SECOND;

	if (index == 0) lcd_clear();
	lcd_put_cur(index, 0);
	sprintf(message, "%s-%s: %ds", orientation, led_state_name, numberDisplay);
	lcd_send_string(message);
}


void trafficLightFSM(const short index){
	// Display duration via UART.
	displayingDuration(index);
	light_counter[index]--;
	switch (ledState[index]){
	case RED:
		//Display only Red LED.
		WritePinLED(index, RED);
		//After amount of time, it will switch to Green.
		if (light_counter[index] <= 0) {
			light_counter[index] = durationGreen * ONE_SECOND;
			ledState[index] = GREEN;
		}
		break;
	case AMBER:
		//Display only Amber LED.
		WritePinLED(index, AMBER);
		//After amount of time, it will switch to Red.
		if (light_counter[index] <= 0) {
			light_counter[index] = durationRed * ONE_SECOND;
			ledState[index] = RED;
		}

		break;
	case GREEN:
		//Display only Green LED.
		WritePinLED(index, GREEN);
		//After amount of time, it will switch to Amber.
		if (light_counter[index] <= 0) {
			light_counter[index] = durationAmber * ONE_SECOND;
			ledState[index] = AMBER;
		}
		break;
	}
}

void pedestrianStateFSM(void){
	switch (pState){
	case SLEEP:
		buzzerNoBeep();
		break;
	case WAIT:
		buzzerNoBeep();
		if (ledState[VERTICAL] == RED && light_counter[VERTICAL] >= 8 * ONE_SECOND){
			pState = ACROSS;
		}
		break;
	case ACROSS:
		if (ledState[VERTICAL] == GREEN || ledState[VERTICAL] == AMBER){
			pState = SLEEP;
		}

		//Peep peep peep
		if (ledState[VERTICAL] == RED) {
			buzzer_counter--;
			if (buzzer_counter <= 0){
				buzzer_counter = setCounterForBuzzer();
				HAL_GPIO_TogglePin(BUZZER_GPIO_Port, BUZZER_Pin);
			}
		}
		break;
	default:
		pState = SLEEP;
	}
}

void runStateFSM(void){
	switch (runState){
		case NORMAL_MODE:
		//Run 2 traffic light FSMs.
		trafficLightFSM(VERTICAL);
		trafficLightFSM(HORIZONTAL);
		pedestrianStateFSM();
		displayPLED();
		break;
	case MODIFY_DURATION_RED_MODE:
		//Blinking Red LED in 0.5s
		blinking_counter--;
		if (blinking_counter <= 0) {
			blinking_counter = HALF_SECOND;
			TogglePinLED(RED);
		}
		break;
	case MODIFY_DURATION_AMBER_MODE:
		//Blinking Amber LED in 0.5s
		blinking_counter--;
		if (blinking_counter <= 0) {
			blinking_counter = HALF_SECOND;
			TogglePinLED(AMBER);
		}
		break;
	case MODIFY_DURATION_GREEN_MODE:
		//Blinking Green LED in 0.5s
		blinking_counter--;
		if (blinking_counter <= 0) {
			blinking_counter = HALF_SECOND;
			TogglePinLED(GREEN);
		}
		break;
	default:
		runState = NORMAL_MODE;
	}
}



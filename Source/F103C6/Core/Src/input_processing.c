/*
 * input_processing.c
 *
 *  Created on: Oct 8, 2023
 *      Author: HP
 */

#include "input_processing.h"

#define DURATION_RED_DEFAULT 		3
#define DURATION_AMBER_DEFAULT 		1
#define DURATION_GREEN_DEFAULT 		2

#define VERTICAL 		0
#define HORIZONTAL 		1
#define DIMENSION		2


ButtonState buttonState[NO_BUTTON];
RunState runState;
LEDState ledState[DIMENSION];
uint16_t light_counter[DIMENSION];

uint8_t durationRed;
uint8_t durationAmber;
uint8_t durationGreen;

uint8_t adjust_duRed;
uint8_t adjust_duAmber;
uint8_t adjust_duGreen;

char msg[64];
UART_HandleTypeDef* UART;

//For blinking led when we switch in modify mode.
int blinking_counter;
//This counter for modify button after amount of time it increase `adjusting variable`.
int increasing_counter;

void resetTrafficLight(void){

}

void toggleLED(void){
	HAL_GPIO_TogglePin(Blinking_LED_GPIO_Port, Blinking_LED_Pin);
}

void transmitMessage(void){
	HAL_UART_Transmit(UART, (uint8_t *) msg, strlen(msg), 500);
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
	ledState[VERTICAL] = GREEN;
	ledState[HORIZONTAL] = RED;

	// Take UART from main.c
	UART = huart;

	// Inform message
	sprintf(msg, "<Pedestrian Project>\r\n");
	transmitMessage();
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
		break;
	case MODIFY_DURATION_AMBER_MODE:
		increaseOne(&adjust_duAmber);
		break;
	case MODIFY_DURATION_GREEN_MODE:
		increaseOne(&adjust_duGreen);
		break;
	}
}
void changingMode(void){
	//Turn off all LEDs.
	//WritePinLED(0, ~RED | ~AMBER | ~GREEN);
	//WritePinLED(1, ~RED | ~AMBER | ~GREEN);

	//Changing state and initial new value for new mode.
	switch(runState){
	case NORMAL_MODE:
		runState = MODIFY_DURATION_RED_MODE;
		blinking_counter = HALF_SECOND;
		adjust_duRed = durationRed;
		break;
	case MODIFY_DURATION_RED_MODE:
		runState = MODIFY_DURATION_AMBER_MODE;
		blinking_counter = HALF_SECOND;
		adjust_duAmber = durationAmber;
		break;
	case MODIFY_DURATION_AMBER_MODE:
		runState = MODIFY_DURATION_GREEN_MODE;
		blinking_counter = HALF_SECOND;
		adjust_duGreen = durationGreen;
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
		break;
	case MODIFY_DURATION_AMBER_MODE:
		durationAmber = adjust_duAmber;
		break;
	case MODIFY_DURATION_GREEN_MODE:
		durationGreen = adjust_duGreen;
		break;
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
	inputProcessingFSM(toggleLED, SET_BTN);
	//inputProcessingFSM(setValue, SET_BTN);
}
void handleModifyButton(void){
	buttonReading(MOD_BTN);
	inputProcessingFSM(toggleLED, MOD_BTN);
	/*
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
	}*/

}
void handleSelectModeButton(void){
	buttonReading(SEL_BTN);
	inputProcessingFSM(toggleLED, SEL_BTN);
	//inputProcessingFSM(changingMode, SEL_BTN);
}

void handlePedestrianButton(void){
	buttonReading(P_BTN);
	inputProcessingFSM(toggleLED, P_BTN);
}

void displayingDuration(uint8_t index){
	if (light_counter[index] % ONE_SECOND != 0 || light_counter[index] == 0)
		return;

	char* orientation;
	if (index == VERTICAL) orientation = "VERTICAL";
	else orientation = "HORIZONTAL";

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

	sprintf(msg, "%s: %ds: %s\r\n", orientation, numberDisplay, led_state_name);
	transmitMessage();
}


void trafficLightFSM(const short index){
	// Display duration via UART.
	displayingDuration(index);
	light_counter[index]--;
	switch (ledState[index]){
	case RED:
		//Display only Red LED.
		//WritePinLED(index, RED);
		//After amount of time, it will switch to Green.
		if (light_counter[index] <= 0) {
			light_counter[index] = durationGreen * ONE_SECOND;
			ledState[index] = GREEN;
		}
		break;
	case AMBER:
		//Display only Amber LED.
		//WritePinLED(index, AMBER);
		//After amount of time, it will switch to Red.
		if (light_counter[index] <= 0) {
			light_counter[index] = durationRed * ONE_SECOND;
			ledState[index] = RED;
		}

		break;
	case GREEN:
		//Display only Green LED.
		//WritePinLED(index, GREEN);
		//After amount of time, it will switch to Amber.
		if (light_counter[index] <= 0) {
			light_counter[index] = durationAmber * ONE_SECOND;
			ledState[index] = AMBER;
		}
		break;
	}

}

void runStateFSM(void){
	switch (runState){
	case NORMAL_MODE:
		//Run 2 traffic light FSMs.
		trafficLightFSM(VERTICAL);
		trafficLightFSM(HORIZONTAL);
		break;
	case MODIFY_DURATION_RED_MODE:
		//Blinking Red LED in 0.5s
		blinking_counter--;
		if (blinking_counter <= 0) {
			blinking_counter = HALF_SECOND;
			//TogglePinLED(RED);
		}
		break;
	case MODIFY_DURATION_AMBER_MODE:
		//Blinking Amber LED in 0.5s
		blinking_counter--;
		if (blinking_counter <= 0) {
			blinking_counter = HALF_SECOND;
			//TogglePinLED(AMBER);
		}
		break;
	case MODIFY_DURATION_GREEN_MODE:
		//Blinking Green LED in 0.5s
		blinking_counter--;
		if (blinking_counter <= 0) {
			blinking_counter = HALF_SECOND;
			//TogglePinLED(GREEN);
		}
		break;
	}
}



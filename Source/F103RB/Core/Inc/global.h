/*
 * global.h
 *
 *  Created on: Nov 30, 2023
 *      Author: HP
 */

#ifndef INC_GLOBAL_H_
#define INC_GLOBAL_H_

#define CYCLE			10
#define ONE_SECOND 		(1000 / CYCLE)
#define HALF_SECOND 	(500 / CYCLE)

#define DURATION_RED_DEFAULT 		12
#define DURATION_AMBER_DEFAULT 		3
#define DURATION_GREEN_DEFAULT 		9

#define VERTICAL 		0
#define HORIZONTAL 		1
#define DIMENSION		2


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
	SLEEP,
	WAIT,
	ACROSS
} PedestrianState;

typedef enum {
	RED = 2,
	AMBER = 3,
	GREEN = 1
} LEDState;


#endif /* INC_GLOBAL_H_ */

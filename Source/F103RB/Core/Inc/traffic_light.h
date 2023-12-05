/*
 * traffic_light.h
 *
 *  Created on: Oct 12, 2023
 *      Author: HP
 */

#ifndef INC_TRAFFIC_LIGHT_H_
#define INC_TRAFFIC_LIGHT_H_

#include "main.h"
#include "input_reading.h"
#include "input_processing.h"
#include "global.h"

void resetAllLED(void);
void WritePinLED(const short index, uint8_t state);
void TogglePinLED(uint8_t state);

#endif /* INC_TRAFFIC_LIGHT_H_ */

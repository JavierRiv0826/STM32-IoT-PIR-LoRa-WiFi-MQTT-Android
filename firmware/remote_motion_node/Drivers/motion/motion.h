/*
 * motion.h
 *
 *  Created on: 18 feb 2026
 *      Author: Xavi
 */

#ifndef MOTION_MOTION_H_
#define MOTION_MOTION_H_

#include "stdint.h"

typedef enum
{
    EVENT_NONE = 0,
    EVENT_MOTION_DETECTED
} event_type_t;

typedef struct
{
    event_type_t type;
    uint32_t timestamp;
} motion_event_t;

extern volatile motion_event_t motion_event;
extern volatile uint8_t motion_detected;

void Motion_Init(void);
void Motion_EXTI_Handler(uint32_t timestamp);

#endif /* MOTION_MOTION_H_ */

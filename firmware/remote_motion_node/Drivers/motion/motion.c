/*
 * motion.c
 *
 *  Created on: 18 feb 2026
 *      Author: Xavi
 */

#include "motion.h"

volatile motion_event_t motion_event;
volatile uint8_t motion_detected = 0;

void Motion_Init(void)
{
	motion_detected = 0;
}

void Motion_EXTI_Handler(uint32_t timestamp)
{
    motion_event.type = EVENT_MOTION_DETECTED;
    motion_event.timestamp = timestamp;
    motion_detected = 1;
}


/*
 * lora.h
 *
 *  Created on: 18 feb 2026
 *      Author: Xavi
 */

#ifndef LORA_LORA_H_
#define LORA_LORA_H_

#include "stdint.h"

extern volatile uint8_t lora_rx_flag;

void LoRa_Init(void);
void LoRa_Send(uint8_t *data, uint8_t length);
void LoRa_SetRxMode(void);
uint8_t LoRa_ReadPacket(uint8_t *buffer);

#endif /* LORA_LORA_H_ */

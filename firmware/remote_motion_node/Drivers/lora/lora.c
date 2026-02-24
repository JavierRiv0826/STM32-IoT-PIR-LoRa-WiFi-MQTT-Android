/*
 * lora.c
 *
 *  Created on: 18 feb 2026
 *      Author: Xavi
 */

#include "lora.h"
#include "main.h"

extern SPI_HandleTypeDef hspi1;

#define LORA_NSS_PORT GPIOA
#define LORA_NSS_PIN  GPIO_PIN_4

#define LORA_RST_PORT GPIOB
#define LORA_RST_PIN  GPIO_PIN_1

volatile uint8_t lora_rx_flag = 0;

static void LORA_Select()
{
    HAL_GPIO_WritePin(LORA_NSS_PORT, LORA_NSS_PIN, GPIO_PIN_RESET);
}

static void LORA_Unselect()
{
    HAL_GPIO_WritePin(LORA_NSS_PORT, LORA_NSS_PIN, GPIO_PIN_SET);
}

static void LORA_WriteReg(uint8_t reg, uint8_t value)
{
    uint8_t buffer[2];
    buffer[0] = reg | 0x80;  // Write bit
    buffer[1] = value;

    LORA_Select();
    HAL_SPI_Transmit(&hspi1, buffer, 2, 100);
    LORA_Unselect();
}

static uint8_t LoRa_ReadReg(uint8_t reg)
{
    uint8_t tx = reg & 0x7F;
    uint8_t rx = 0;

    LORA_Select();
    HAL_SPI_Transmit(&hspi1, &tx, 1, 100);
    HAL_SPI_Receive(&hspi1, &rx, 1, 100);
    LORA_Unselect();

    return rx;
}

void LoRa_Init(void)
{
	// Reset module
	HAL_GPIO_WritePin(LORA_RST_PORT, LORA_RST_PIN, GPIO_PIN_RESET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(LORA_RST_PORT, LORA_RST_PIN, GPIO_PIN_SET);
	HAL_Delay(10);

	// Sleep
	LORA_WriteReg(0x01, 0x80);

	// LoRa + Standby
	LORA_WriteReg(0x01, 0x81);

	// Frequency 433.000 MHz
	LORA_WriteReg(0x06, 0x6C);
	LORA_WriteReg(0x07, 0x40);
	LORA_WriteReg(0x08, 0x00);

	// PA Boost 14dBm
	LORA_WriteReg(0x09, 0x8E);

	// LNA boost
	LORA_WriteReg(0x0C, 0x23);

	// Modem config
	LORA_WriteReg(0x1D, 0x72);   // BW125 CR4/5 explicit
	LORA_WriteReg(0x1E, 0x74);   // SF7 CRC ON
	LORA_WriteReg(0x26, 0x04);   // AGC auto, no low data rate optimize

	// Preamble
	LORA_WriteReg(0x20, 0x00);
	LORA_WriteReg(0x21, 0x08);

	// Sync word
	LORA_WriteReg(0x39, 0x34);

	// Clear IRQ
	LORA_WriteReg(0x12, 0xFF);
}

void LoRa_Send(uint8_t *data, uint8_t length)
{
    LORA_WriteReg(0x0E, 0x00);  // FIFO TX base
    LORA_WriteReg(0x0D, 0x00);  // FIFO pointer

    for(uint8_t i = 0; i < length; i++)
    {
        LORA_WriteReg(0x00, data[i]);
    }

    LORA_WriteReg(0x22, length);  // Payload length
    LORA_WriteReg(0x01, 0x83);    // TX mode

    // Wait for TX done
    while((LoRa_ReadReg(0x12) & 0x08) == 0);

    // Clear TX done flag
    LORA_WriteReg(0x12, 0x08);
}

void LoRa_SetRxMode(void)
{
    // Set FIFO RX base address
    LORA_WriteReg(0x0F, 0x00);

    // LNA boost
    LORA_WriteReg(0x0C, 0x23);

    // Clear IRQ flags
    LORA_WriteReg(0x12, 0xFF);

    // Continuous RX mode
    LORA_WriteReg(0x01, 0x85);

    // Map DIO0 to RxDone
    LORA_WriteReg(0x40, 0x00);
}

uint8_t LoRa_ReadPacket(uint8_t *buffer)
{
    uint8_t length = LoRa_ReadReg(0x13);  // Number of received bytes

    uint8_t fifoAddr = LoRa_ReadReg(0x10);
    LORA_WriteReg(0x0D, fifoAddr);

    for(uint8_t i = 0; i < length; i++)
    {
        buffer[i] = LoRa_ReadReg(0x00);
    }

    return length;
}

# 📡 STM32 LoRa Motion Node

📌 Description<br>
A modular embedded systems project built on the STM32F103C6T6 (Blue Pill) featuring motion detection with a PIR sensor and wireless transmission via LoRa SX1278 (433 MHz). This node communicates motion events to a gateway node and can receive basic commands (like LED control) over LoRa.

The firmware uses a layered architecture for maintainable, non-blocking design:
* Motion driver (PIR sensor + EXTI interrupts)
* LoRa driver (SPI communication + SX1278 configuration)
* Application layer (event handling, LED feedback)

This project demonstrates clean embedded design, non-blocking communication, and scalable firmware architecture for IoT edge nodes.

---
## 🚀 Features
### 🕵️ Motion Detection
* ✔ PIR sensor triggers external interrupt on PA1
* ✔ Motion events timestamped with TIM2 (1 kHz)
* ✔ LED feedback on motion detection (PC13)
### 📡 LoRa Communication
* ✔ SX1278 433 MHz module (SPI interface)
* ✔ LoRa half-duplex: send motion event + return to RX mode
* ✔ Payload format for motion event:

  | Byte | Description              |
  |------|--------------------------|
  | 0    | Node ID (0x01)           |
  | 1    | Event ID (0x01 = motion) |
  | 2-5  | Timestamp (uint32_t)     |
* ✔ Incoming command example: LED control ([NodeID, CMD_ID=0x10, VALUE])
### 🧠 Application Layer
* ✔ Motion event detection
* ✔ LoRa transmission & reception
* ✔ LED visual feedback
* ✔ Non-blocking main loop

---
# ⚙️ STM32CubeMX Configuration

## 🔄 Clock
* HSE = 8 MHz
* PLL ×9 → SYSCLK = 72 MHz
* AHB = 72 MHz
* APB1 = 36 MHz
* APB2 = 72 MHz

## 🔗 SPI1 (LoRa)
* Mode: Master
* DataSize: 8-bit
* NSS → PA4
* SCK → PA5
* MISO → PA6
* MOSI → PA7

## 🔁 TIM2 (System Timer)
* Prescaler = 71 → 1 ms tick
* Period = 999
* Used for motion event timestamps

## ↔️ GPIO
* PA1 → PIR sensor (EXTI rising)
* PC13 → Onboard LED (motion)
* PA2 → Optional LED (command)
* PA4 → LoRa NSS
* PB1 → LoRa RST
* PB0 → LoRa DIO0 (RX interrupt)

---
## 🔌 Wiring (PIR Sensor → STM32)
| PIR | STM32F103 |
|-----|-----------|
| VCC | 5V        |
| GND | GND       |
| OUT | PA1       |

## 🔌 Wiring (SX1278 LoRa Module → STM32)
| SX1278 | STM32F103 |
|--------|-----------|
| VCC    | 3.3V      |
| GND    | GND       |
| NSS    | PA4       |
| RST    | PB1       |
| DIO0   | PB0       |
| SCK    | PA5       |
| MISO   | PA6       |
| MOSI   | PA7       |

> ⚠️ Ensure SPI pins match CubeIDE configuration.

---
## 📦 Sensor Overview
### PIR Motion Sensor – HC-SR501    
* Interface: Digital Output (GPIO with interrupt)
* Trigger: Rising edge
* Voltage: 5V
* Current: <50 μA standby, ~10 mA active
* Detection Range: ~3–7 meters (adjustable via potentiometer)
* Detection Angle: ~110°
### Used values in firmware:
* motion_event.type → EVENT_MOTION_DETECTED
* motion_event.timestamp → uint32_t (milliseconds since boot)
### Notes:
* Sensor connected to PA1 as an external interrupt (EXTI).
* On motion detection, the firmware updates motion_detected = 1 and sends a LoRa packet with the timestamp.

## 📦 LoRa SX1278 Module Overview
### LoRa SX1278 – 433 MHz Transceiver
* Interface: SPI
* Voltage: 3.3V (do not exceed 3.6V)
* Frequency: 433 MHz (configurable)
* Modulation: LoRa (Long Range, Low Power)
* Transmit Power: up to +14 dBm (PA Boost)
* Sensitivity: -137 dBm (SF12, 125 kHz)
* Current Consumption:
    * TX: 28 mA (@ +14 dBm)
    * RX: 10.3 mA
    * Sleep: 1 μA
### Key Firmware Notes:
* LoRa_Init() configures SX1278 with SF7, BW125 kHz, CRC enabled
* LoRa_Send() transmits payloads in TX mode
* LoRa_SetRxMode() switches module back to continuous RX for command reception
* DIO0 interrupt triggers lora_rx_flag when a packet is received
### Remarks:
* SX1278 is half-duplex, cannot TX and RX simultaneously
* Frequency, spreading factor, bandwidth, and power can be adjusted in lora.c
* Ideal for low-power IoT nodes requiring long-range communication

---
## 🔐 Payload & Commands
### Motion Event (6 Bytes)
| Byte | Description              |
|------|--------------------------|
| 0    | Node ID (0x01)           |
| 1    | Event ID (0x01 = motion) |
| 2-5  | Timestamp (uint32_t)     |
### Command Payload Example
| NodeID | CMD_ID | VALUE          |
|--------|--------|----------------|
| 0x01   | 0x10   | 0x01 → LED ON  |
| 0x01   | 0x10   | 0x00 → LED OFF |

## 🧩 Drivers Overview
### motion.c / motion.h
* Handles motion detection via PIR and EXTI interrupts
* Updates volatile motion_event_t motion_event and volatile uint8_t motion_detected
### lora.c / lora.h
* SPI driver for SX1278 LoRa module
* Functions:
    * LoRa_Init() – configure LoRa module
    * LoRa_Send(uint8_t *data, uint8_t length) – transmit payload
    * LoRa_SetRxMode() – return to continuous RX
    * uint8_t LoRa_ReadPacket(uint8_t *buffer) – read received payload
---
## 🏗 Firmware Architecture
```bash
+----------------------------+
| Application Layer          |
| - Event handling           |
| - LED feedback             |
| - LoRa TX/RX orchestration |
+----------------------------+
             ↓
+----------------------------+
| Motion Driver              |
| - motion.c / motion.h      |
| - EXTI callback handling   |
+----------------------------+
             ↓
+----------------------------+
| LoRa Driver                |
| - lora.c / lora.h          |
| - SPI communication        |
| - SX1278 initialization    |
| - Packet send/receive      |
+----------------------------+
```
main.c orchestrates modules — no blocking code inside main loop except HAL delays for LED feedback.

---
## 🗂 Project Structure

```bash
project/
├── drivers/
│   ├── motion/
│   │   ├── motion.c
│   │   └── motion.h
│   │
│   └── lora/
│       ├── lora.c
│       └── lora.h
│
├── Core/              
│   ├── Inc/
│   │   └── main.h
│   └── Src/
│       └── main.c
│
└── STM32CubeIDE files
```

---
## 💡 How to Run 🧪

### 1. Clone the Repository
```bash
git clone https://github.com/JavierRiv0826/STM32-IoT-PIR-LoRa-WiFi-MQTT-Android.git
```
### 2. Open in STM32CubeIDE
* File → Open Projects from File System
* Select the project folder

### 3. Build the Project
Press **Ctrl + B**  
or  
Project → Build Project

### 4. Flash the Microcontroller
* Connect ST-Link or USB-to-Serial (bootloader mode)
* Press **Debug** or **Run*

### 5. Node automatically enters LoRa RX mode
### 6. Trigger motion → LED blinks and event sent
### 7. Optional: Send commands from the gateway to control PA2 LED.

---
## 🧩 Hardware Overview

### **Microcontroller**
* STM32F103C6
* ARM Cortex-M3 @ 72 MHz
* 32 KB Flash, 10 KB RAM

## 🛠 Development Tools
* **STM32CubeIDE**
* **STM32CubeMX**

---

## 👤 Author
**Javier Rivera**  
GitHub: *JavierRiv0826*

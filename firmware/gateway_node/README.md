# 🌐 ESP8266 LoRa → WiFi → MQTT Gateway
📌 Description<br>
This project implements a LoRa-to-WiFi gateway using the ESP8266 (NodeMCU v2).

It receives motion events from a remote STM32 LoRa node and forwards them to an MQTT broker over WiFi. It also listens for MQTT commands and forwards them back to the LoRa node.

This gateway acts as a bridge between:
* 📡 LoRa network (433 MHz)
* 🌐 WiFi network
* ☁️ MQTT broker (cloud/local)

---
## 🚀 Features
### 📡 LoRa Receiver
* ✔ Receives 6-byte motion packets
* ✔ Reads RSSI for signal strength monitoring
* ✔ LED indicator on packet reception
* ✔ Uses SX1278 via SPI
### 🌐 WiFi Connectivity
* ✔ Connects to configured SSID
* ✔ Auto-reconnect on disconnect
* ✔ Non-blocking loop
### ☁️ MQTT Integration
* ✔ Publishes motion events as JSON
* ✔ Subscribes to command topic
* ✔ Forwards MQTT commands to LoRa
* ✔ Uses public broker (HiveMQ by default)
---
## ⚙️ PlatformIO Configuration
### platformio.ini
```bash
[env:nodemcuv2]
platform = espressif8266
board = nodemcuv2
framework = arduino
monitor_speed = 115200

lib_deps =
    sandeepmistry/LoRa
    knolleary/PubSubClient
```
### 📚 Libraries Used
* LoRa by Sandeep Mistry → SX1278 driver
* PubSubClient → MQTT client
* ESP8266WiFi → WiFi connectivity

---
## 📡 LoRa SX1278 Module Overview
### SX1278 – 433 MHz LoRa Transceiver
* Interface: SPI
* Voltage: 3.3V (⚠️ do not exceed 3.6V)
* Frequency: 433 MHz
* Spreading Factor: 7 (configured)
* Bandwidth: 125 kHz
* Coding Rate: 4/5
* CRC: Enabled
* Sync Word: 0x34

## 🔌 Wiring
### ESP8266 (NodeMCU v2) → SX1278
| SX1278 | ESP8266 Pin |
|--------|-------------|
| VCC    | 3.3V        |
| GND    | GND         |
| NSS    | D8          |
| RST    | D1          |
| DIO0   | D2          |
| SCK    | D5          |
| MISO   | D6          |
| MOSI   | D7          |
> ⚠️ SX1278 is 3.3V only. Never use 5V.

## 📦 LoRa Packet Overview
### Motion Event Packet (6 Bytes)
| Byte | Field     | Type     |
|------|-----------|----------|
| 0    | Node ID   | uint8_t  |
| 1    | Event ID  | uint8_t  |
| 2-5  | Timestamp | uint32_t |
When a packet is received:
* LED toggles
* Timestamp is converted to mm:ss
* RSSI is measured
* JSON message is published to MQTT

---

## ☁️ MQTT Configuration
```bash
const char* mqtt_server = "broker.hivemq.com";

const char* topic_sub = 
"javierriv0826_esp8266_2026/lora/command";

const char* topic_pub = 
"javierriv0826_esp8266_2026/lora/motion";
```

## 📤 Published JSON Format
### Example MQTT message:
```bash
{
  "node": 1,
  "event": 1,
  "timestamp": 123456,
  "rssi": -72
}
```

## 📥 Command Format (MQTT → LoRa)
### MQTT payload must contain 3 raw bytes:
| Byte | Description |
|------|-------------|
| 0    | Node ID     |
| 1    | CMD ID      |
| 2    | Value       |

Example:

| NodeID | CMD_ID | VALUE          |
|--------|--------|----------------|
| 0x01   | 0x10   | 0x01 → LED ON  |
| 0x01   | 0x10   | 0x00 → LED OFF |

After sending the packet:

```bash
LoRa.receive();  // Return to RX mode
```
> ⚠️ This is critical because LoRa is half-duplex.

---
## 🏗 Gateway Architecture
```bash
+--------------------------+
| LoRa Layer               |
| - parsePacket()          |
| - RSSI reading           |
+--------------------------+
             ↓
+--------------------------+
| Application Layer        |
| - Packet parsing         |
| - Timestamp formatting   |
| - JSON building          |
+--------------------------+
             ↓
+--------------------------+
| MQTT Layer               |
| - Publish motion events  |
| - Subscribe to commands  |
+--------------------------+
             ↓
+--------------------------+
| WiFi Layer               |
| - Auto reconnect         |
+--------------------------+
```
The loop remains non-blocking:
* client.loop() handles MQTT
* LoRa.parsePacket() checks radio
* No delay-based blocking logic

---
## 🧪 How to Run

Clone the Repository
```bash
git clone https://github.com/JavierRiv0826/STM32-IoT-PIR-LoRa-WiFi-MQTT-Android.git
```

### 1️⃣ Install VS Code
### 2️⃣ Install PlatformIO extension
### 3️⃣ Open project folder
### 4️⃣ Edit WiFi credentials:
```bash
const char* ssid = "YOUR_WIFI";
const char* password = "YOUR_PASS";
```
### 5️⃣ Upload to ESP8266
* Connect NodeMCU via USB
* Click Upload
* Open Serial Monitor (115200 baud)
### 6️⃣ Trigger motion on STM32 node
You should see:
```bash
📡 Motion detected at 123456 -> 2:03
```
And MQTT will publish the JSON message.

---
## 🛠 Development Tools
* **VS Code**
* **PlatformIO**

---

## 👤 Author
**Javier Rivera**  
GitHub: *JavierRiv0826*
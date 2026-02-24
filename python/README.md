# 🐍 Python MQTT Monitor & Control Client
This Python application connects to the MQTT broker used by the Motion LoRa IoT Gateway, allowing:
* 📡 Real-time motion event monitoring
* 🎮 Remote command transmission to LoRa nodes
* 🖥️ Interactive keyboard-based control

It acts as a desktop control panel for testing and debugging the IoT system.

---
## 🚀 Features
* MQTT v5 compatible client
* JSON payload decoding for motion events
* Binary command transmission to remote node
* Non-blocking keyboard listener
* Clean shutdown handling

---
## 🌐 MQTT Configuration
| Parameter       | Value                                     |
|-----------------|-------------------------------------------|
| Broker          | `broker.hivemq.com`                       |
| Port            | 1883                                      |
| Subscribe Topic | `javierriv0826_esp8266_2026/lora/motion`  |
| Command Topic   | `javierriv0826_esp8266_2026/lora/command` |
| Node ID         | `0x01`                                    |

## 📩 Motion Event Payload (Received)
The Gateway publishes motion events in JSON format:
```bash
{
  "node": 1,
  "event": 1,
  "timestamp": 1700000000,
  "rssi": -92
}
```
## Decoded Fields
| Field       | Description          |
|-------------|----------------------|
| `node`      | Node ID              |
| `event`     | Motion event flag    |
| `timestamp` | Unix timestamp       |
| `rssi`      | LoRa signal strength |

## 🎮 Command System
Commands are sent as binary payloads (3 bytes):
```bash
[ NODE_ID | CMD_ID | VALUE ]
```
### Implemented Commands
| Key | Command  | CMD ID | Value |
|-----|----------|--------|-------|
| `1` | LED ON   | 0x10   | 0x01  |
| `0` | LED OFF  | 0x10   | 0x00  |
| `r` | RESET    | 0x12   | 0x01  |
| `q` | Exit app | —      | —     |

Example transmitted payload (LED ON):
```bash
01 10 01
```
---
## 🏗️ System Architecture
```bash
Remote Motion Node (STM32 + SX1278)
            │
            ▼
      LoRa Transmission
            │
            ▼
Gateway Node (ESP8266/ESP32)
            │
            ▼
        MQTT Broker
            │
            ▼
     Python Desktop Client
```
---
## 🧰 Requirements
- Python 3.7+
- mqtt
### Dependencies
```bash
pip install paho-mqtt pynput
```

---
## 💡 How to Run 🧪

### 1. Clone the Repository
```bash
git clone https://github.com/JavierRiv0826/STM32-IoT-PIR-LoRa-WiFi-MQTT-Android.git
```
### 2. Run the script:
```bash
python MQTT_monitor_tool.py
```
Expected output:
```bash
Connected to broker MQTT
System READY:
  1 → LED ON
  0 → LED OFF
  r → RESET
```

### 3. Press "1","0" keys from keyboard to send commands.

---
## 🧠 How It Works
### MQTT
* Uses paho.mqtt.client
* Runs in non-blocking loop mode (loop_start())
### Keyboard Handling
* Uses pynput
* Listener runs in a separate thread
* Does not block MQTT reception
### Clean Shutdown
* Stops listener
* Stops MQTT loop
* Disconnects from broker

---

# 🔐 Security Note ⚠️
## Currently using:
### * Public MQTT broker
### * No TLS
### * No authentication
## For production deployment:
### * Enable TLS (port 8883)
### *  Use username/password
### *  Use private broker

---

## 👤 Author
**Javier Rivera**  
GitHub: *JavierRiv0826*
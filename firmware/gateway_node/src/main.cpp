#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <LoRa.h>

// ---------- WIFI ----------
const char* ssid = "YOUR_WIFI";
const char* password = "YOUR_PASS";

// ---------- MQTT ----------
//const char* mqtt_server = "test.mosquitto.org";
const char* mqtt_server = "broker.hivemq.com";
const char* topic_sub  = "javierriv0826_esp8266_2026/lora/command";
const char* topic_pub  = "javierriv0826_esp8266_2026/lora/motion";

WiFiClient espClient;
PubSubClient client(espClient);

// ---------- LORA PINS ----------
#define SS   D8
#define RST  D1
#define DIO0 D2

#define LORA_FREQ 433E6   // Change if needed

bool ledState = false;

// ---------- FUNCTION DECLARATIONS ----------
void setup_wifi();
void reconnect_mqtt();
void mqtt_callback(char* topic, byte* payload, unsigned int length);

void setup() {

  Serial.begin(115200);
  delay(1000);

  setup_wifi();

  client.setServer(mqtt_server, 1883);
  client.setCallback(mqtt_callback);

  // LoRa init
  LoRa.setPins(SS, RST, DIO0);

  if (!LoRa.begin(LORA_FREQ)) {
    Serial.println("❌ LoRa init failed");
    while (1);
  }

  LoRa.setSpreadingFactor(7);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(5);
  LoRa.enableCrc();
  LoRa.setSyncWord(0x34);
  LoRa.setTxPower(10);

  pinMode(LED_BUILTIN, OUTPUT);

  Serial.println("✅ LoRa init OK");
}

void loop() {

  if (!client.connected()) {
    reconnect_mqtt();
  }

  client.loop();

  int packetSize = LoRa.parsePacket();

  if (packetSize > 0) {
    // Toggle LED
    ledState = !ledState;
    digitalWrite(LED_BUILTIN, ledState ? LOW : HIGH);
  }

  if (packetSize == 6) {  // NodeID + EventID + 4 bytes timestamp

    uint8_t buffer[6];

    for (int i = 0; i < 6; i++) {
      buffer[i] = LoRa.read();
    }

    uint8_t nodeId = buffer[0];
    uint8_t eventId = buffer[1];

    uint32_t timestamp =
      ((uint32_t)buffer[2] << 24) |
      ((uint32_t)buffer[3] << 16) |
      ((uint32_t)buffer[4] << 8)  |
      ((uint32_t)buffer[5]);

    int rssi = LoRa.packetRssi();

    uint32_t totalSec = timestamp / 1000;
    uint32_t min = totalSec / 60;
    uint32_t sec = totalSec % 60;

    Serial.print("📡 Motion detected at ");
    Serial.print(timestamp);
    Serial.print(" -> ");
    Serial.print(min);
    Serial.print(":");
    if (sec < 10) Serial.print("0");
    Serial.println(sec);

    String json = "{";
    json += "\"node\":" + String(nodeId) + ",";
    json += "\"event\":" + String(eventId) + ",";
    json += "\"timestamp\":" + String(timestamp) + ",";
    json += "\"rssi\":" + String(rssi);
    json += "}";

    client.publish(topic_pub, json.c_str());
  }
}

// ---------------- WIFI ----------------

void setup_wifi() {

  Serial.println("Connecting WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n✅ WiFi connected");
}

// ---------------- MQTT ----------------

void reconnect_mqtt() {

  while (!client.connected()) {

    Serial.println("Connecting MQTT...");

    if (client.connect("ESP8266_Gateway")) {

      Serial.println("✅ MQTT connected");
      client.subscribe(topic_sub);

    } else {
      Serial.println("Retrying MQTT...");
      delay(2000);
    }
  }
}

// ----------- MQTT CALLBACK ------------

void mqtt_callback(char* topic, byte* payload, unsigned int length) {

  Serial.println("📥 MQTT command received");

  if (length < 3) return;

  uint8_t nodeId = payload[0];
  uint8_t cmdId  = payload[1];
  uint8_t value  = payload[2];

  Serial.print("Forwarding to LoRa → CMD: ");
  Serial.println(cmdId, HEX);

  // Send via LoRa
  LoRa.beginPacket();
  LoRa.write(nodeId);
  LoRa.write(cmdId);
  LoRa.write(value);
  LoRa.endPacket();

  // Toggle LED
  ledState = !ledState;
  digitalWrite(LED_BUILTIN, ledState ? LOW : HIGH);

  LoRa.receive();  // 🔥 VERY IMPORTANT: back to RX mode
}
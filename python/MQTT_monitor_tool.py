import json
import time
from pynput import keyboard
import paho.mqtt.client as mqtt

#BROKER = "test.mosquitto.org"
BROKER = "broker.hivemq.com"
PORT = 1883

TOPIC_SUB = "javierriv0826_esp8266_2026/lora/motion"
TOPIC_CMD = "javierriv0826_esp8266_2026/lora/command"

NODE_ID = 0x01

# ---------------- MQTT CALLBACKS ----------------

def on_connect(client, userdata, flags, reason_code, properties):
    if reason_code == 0:
        print("✅ Conected to broker MQTT")
        client.subscribe(TOPIC_SUB)
    else:
        print("❌ Connection Error:", reason_code)


def on_message(client, userdata, msg):
    print("\n📩 Message received")

    try:
        payload = json.loads(msg.payload.decode())
        print("Node       :", payload.get("node"))
        print("Event      :", payload.get("event"))
        print("Timestamp  :", payload.get("timestamp"))
        print("RSSI       :", payload.get("rssi"))
    except:
        print("Raw        :", msg.payload)


# ---------------- MQTT SETUP ----------------

client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
client.on_connect = on_connect
client.on_message = on_message

print("Conecting to broker...")
client.connect(BROKER, PORT, 60)
client.loop_start()  # No blocking


# ---------------- COMMAND FUNTION ----------------

def send_command(cmd_id, value):
    payload = bytes([NODE_ID, cmd_id, value])
    client.publish(TOPIC_CMD, payload)
    print(f"\n🚀 Command sent → CMD: {hex(cmd_id)} VAL: {value}")


# ---------------- KEYBOARD ----------------

def on_press(key):
    try:
        if key.char == '1':
            send_command(0x10, 0x01)  # LED ON

        elif key.char == '0':
            send_command(0x10, 0x00)  # LED OFF

        elif key.char == 'r':
            send_command(0x12, 0x01)  # RESET

        elif key.char == 'q':
            print("\nExit...")
            return False

    except AttributeError:
        pass


listener = keyboard.Listener(on_press=on_press)
listener.start()


# ---------------- MAIN LOOP ----------------

try:
    print("\nSystem READY:")
    print("  1 → LED ON")
    print("  0 → LED OFF")
    print("  r → RESET")
    print("  Ctrl+C to stop\n")

    while True:
        time.sleep(1)

except KeyboardInterrupt:
    print("Application terminated.")

finally:
    listener.stop()
    client.loop_stop()
    client.disconnect()
    print("Application terminated.")

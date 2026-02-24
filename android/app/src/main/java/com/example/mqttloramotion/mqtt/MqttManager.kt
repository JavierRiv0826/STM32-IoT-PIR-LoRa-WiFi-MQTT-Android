package com.example.mqttloramotion.mqtt

import android.content.Context
import android.util.Log
import org.eclipse.paho.android.service.MqttAndroidClient
import org.eclipse.paho.client.mqttv3.*

class MqttManager(
    context: Context,
    private val onMessageReceived: (String) -> Unit
) {

    private val broker = "tcp://broker.hivemq.com:1883"
    private val clientId = MqttClient.generateClientId()

    private val topicSub = "javierriv0826_esp8266_2026/lora/motion"
    private val topicCmd = "javierriv0826_esp8266_2026/lora/command"

    private val nodeId: Byte = 0x01

    private val client = MqttAndroidClient(context, broker, clientId)

    fun connect() {
        val options = MqttConnectOptions().apply {
            isAutomaticReconnect = true
            isCleanSession = true
        }

        client.setCallback(object : MqttCallback {

            override fun connectionLost(cause: Throwable?) {
                Log.e("DB_MQTT", "Connection lost", cause)
            }

            override fun messageArrived(topic: String?, message: MqttMessage?) {
                Log.d("DB_MQTT", "Message arrived on $topic")
                message?.let {
                    val payload = String(it.payload)
                    onMessageReceived(payload)
                }
            }

            override fun deliveryComplete(token: IMqttDeliveryToken?) {
                Log.d("DB_MQTT", "Delivery complete")
            }
        })

        Log.d("DB_MQTT", "Connecting to broker...")

        client.connect(options, null, object : IMqttActionListener {
            override fun onSuccess(asyncActionToken: IMqttToken?) {
                Log.d("DB_MQTT", "Connected successfully")
                client.subscribe(topicSub, 1)
            }

            override fun onFailure(asyncActionToken: IMqttToken?, exception: Throwable?) {
                Log.e("DB_MQTT", "Connection failed", exception)
            }
        })
    }

    fun sendCommand(cmdId: Byte, value: Byte) {
        val payload = byteArrayOf(nodeId, cmdId, value)
        val message = MqttMessage(payload)
        client.publish(topicCmd, message)
    }
}
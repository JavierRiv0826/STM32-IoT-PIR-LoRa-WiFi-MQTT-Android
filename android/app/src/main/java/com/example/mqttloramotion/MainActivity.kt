package com.example.mqttloramotion

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.material3.Button
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Text
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp
import com.example.mqttloramotion.mqtt.MqttManager
import com.example.mqttloramotion.ui.theme.MQTTLoraMotionTheme
import org.json.JSONObject

class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        setContent {
            MQTTLoraMotionTheme {

                var lastMessage by remember { mutableStateOf("Waiting for motion...") }

                val mqttManager = remember {
                    MqttManager(this) { message ->
                        lastMessage = formatMessage(message)
                    }
                }

                LaunchedEffect(Unit) {
                    mqttManager.connect()
                }

                Scaffold(modifier = Modifier.fillMaxSize()) { innerPadding ->

                    Column(
                        modifier = Modifier
                            .fillMaxSize()
                            .padding(innerPadding),
                        verticalArrangement = Arrangement.Center,
                        horizontalAlignment = Alignment.CenterHorizontally
                    ) {

                        Text("📡 Motion Monitor", style = MaterialTheme.typography.headlineMedium)

                        Spacer(modifier = Modifier.height(24.dp))

                        Text("Last Event:")
                        Spacer(modifier = Modifier.height(8.dp))
                        Text(lastMessage)

                        Spacer(modifier = Modifier.height(40.dp))

                        Button(onClick = {
                            mqttManager.sendCommand(0x10, 0x01)
                        }) {
                            Text("LED ON")
                        }

                        Spacer(modifier = Modifier.height(16.dp))

                        Button(onClick = {
                            mqttManager.sendCommand(0x10, 0x00)
                        }) {
                            Text("LED OFF")
                        }

                        Spacer(modifier = Modifier.height(16.dp))

                        Button(onClick = {
                            mqttManager.sendCommand(0x12, 0x01)
                        }) {
                            Text("RESET")
                        }
                    }
                }
            }
        }
    }
}

private fun formatMessage(payload: String): String {
    return try {
        val json = JSONObject(payload)

        val event = json.getInt("event")
        val timestampMs = json.getLong("timestamp")
        val rssi = json.getInt("rssi")

        // Convert milliseconds to HH:mm:ss (uptime style)
        val totalSec = timestampMs / 1000
        val hours = totalSec / 3600
        val minutes = (totalSec % 3600) / 60
        val seconds = totalSec % 60

        val timeString = String.format("%02d:%02d:%02d", hours, minutes, seconds)

        // Translate event
        val eventText = when (event) {
            1 -> "🚨 MOTION"
            else -> "ℹ️ EVENT $event"
        }

        // Signal quality
        val signalQuality = when {
            rssi >= -60 -> "📡 Excellent signal"
            rssi >= -70 -> "📡 Good signal"
            rssi >= -80 -> "📡 Fair signal"
            else -> "📡 Weak signal"
        }

        """
        📩 EVENT REPORT
        
        $eventText
        🕒 Time: $timeString
        📶 Signal: $rssi dBm
        
        $signalQuality
        """.trimIndent()

    } catch (_: Exception) {
        "Invalid message:\n$payload"
    }
}
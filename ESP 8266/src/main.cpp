#include <WiFi.h>
#include <WebSocketsClient.h>
#include <esp_camera.h>
#include <ArduinoJson.h>

// Configuración WiFi
const char* ssid = "Casa Bello";
const char* password = "N66k8nbe";

// Configuración WebSocket
const char* serverIp = "mk-bot-online.onrender.com"; // Cambia por la IP del servidor
const int serverPort = 443;

WebSocketsClient webSocket;

// Pines de los motores
const int ENA = 5;
const int IN1 = 18;
const int IN2 = 19;
const int IN3 = 4;
const int IN4 = 23;
const int ENB = 22;

// Identificador del robot
const char* robotID = "robot1";

// Configuración de la cámara
#define CAMERA_MODEL_AI_THINKER
#include "camera_pins.h"

// Función para capturar y enviar imagen por WebSocket
void captureAndSendFrame() {
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
        Serial.println("Error al capturar imagen");
        return;
    }

    Serial.println("Enviando imagen al servidor...");
    webSocket.sendBIN(fb->buf, fb->len);
    esp_camera_fb_return(fb);
}

// Función para procesar comandos WebSocket
void webSocketEvent(WStype_t type, uint8_t* payload, size_t length) {
    switch (type) {
    case WStype_DISCONNECTED:
        Serial.println("Desconectado del servidor WebSocket");
        break;
    case WStype_CONNECTED: {
        Serial.println("Conectado al servidor WebSocket");
        String registerMessage = String("{\"clase\":\"robot\",\"ID\":\"") + robotID + "\"}";
        Serial.println("Enviando mensaje de registro: " + registerMessage);
        webSocket.sendTXT(registerMessage);
        break;
    }
    case WStype_TEXT: {
        Serial.println("Mensaje recibido desde WebSocket");
        String message = String((char*)payload);
        Serial.println("Mensaje: " + message);

        DynamicJsonDocument doc(256);
        DeserializationError error = deserializeJson(doc, message);

        if (error) {
            Serial.println("Error al analizar el mensaje JSON");
            return;
        }

        const char* command = doc["command"];
        if (command) {
            String cmd = String(command);

            if (cmd == "adelante") {
                Serial.println("Mover adelante");
                digitalWrite(IN1, HIGH);
                digitalWrite(IN2, LOW);
                digitalWrite(IN3, HIGH);
                digitalWrite(IN4, LOW);
            } else if (cmd == "atras") {
                Serial.println("Mover atrás");
                digitalWrite(IN1, LOW);
                digitalWrite(IN2, HIGH);
                digitalWrite(IN3, LOW);
                digitalWrite(IN4, HIGH);
            } else if (cmd == "izquierda") {
                Serial.println("Girar izquierda");
                digitalWrite(IN1, LOW);
                digitalWrite(IN2, HIGH);
                digitalWrite(IN3, HIGH);
                digitalWrite(IN4, LOW);
            } else if (cmd == "derecha") {
                Serial.println("Girar derecha");
                digitalWrite(IN1, HIGH);
                digitalWrite(IN2, LOW);
                digitalWrite(IN3, LOW);
                digitalWrite(IN4, HIGH);
            } else if (cmd == "stop") {
                Serial.println("Detener");
                digitalWrite(IN1, LOW);
                digitalWrite(IN2, LOW);
                digitalWrite(IN3, LOW);
                digitalWrite(IN4, LOW);
            }
        }
        break;
    }
    }
}

void setup() {
    Serial.begin(115200);

    pinMode(ENA, OUTPUT);
    pinMode(ENB, OUTPUT);
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Conectando a WiFi...");
    }
    Serial.println("Conectado a WiFi");

    webSocket.beginSSL(serverIp, serverPort, "/");
    webSocket.onEvent(webSocketEvent);
}

void loop() {
    webSocket.loop();
    captureAndSendFrame(); 
    delay(100); 
}

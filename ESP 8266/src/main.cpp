#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include <arduino.h>

// Configuración WiFi
const char* ssid = "Casa Bello";
const char* password = "N66k8nbe";

// IP del Servidor
const char* serverIp = "mk-bot-online.onrender.com"; // Cambia por la IP del servidor
const int serverPort = 443;

WebSocketsClient webSocket;

// Motor A - Derecho
const int ENA = D6;  // ENA conectado al pin digital 10
const int IN1 = D1;  // IN1 conectado al pin digital 9
const int IN2 = D2;  // IN2 conectado al pin digital 8

// Motor B - Izquierdo
const int IN3 = D3;  // IN3 conectado al pin digital 7
const int IN4 = D4;  // IN4 conectado al pin digital 6
const int ENB = D5;  // ENB conectado al pin digital 5

// Identificador del robot
const char* robotID = "robot1";

void webSocketEvent(WStype_t type, uint8_t* payload, size_t length) {
    switch (type) {
    case WStype_DISCONNECTED:
        Serial.println("Desconectado del servidor");
        break;
    case WStype_CONNECTED: {
        Serial.println("Conectado al servidor WebSocket");
        // Registrarse con el servidor en formato JSON
        String registerMessage = String("{\"clase\":\"robot\",\"ID\":\"") + robotID + "\"}";
        webSocket.sendTXT(registerMessage);
        break;
    }
    case WStype_TEXT: {
    Serial.println("Mensaje recibido desde WebSocket");
    String message = String((char*)payload);
    Serial.println("Mensaje: " + message);

    // Analizar mensaje JSON
    DynamicJsonDocument doc(256);
    DeserializationError error = deserializeJson(doc, message);

    if (error) {
        Serial.println("Error al analizar el mensaje JSON");
        return;
    }

    const char* id = doc["ID"];
    const char* command = doc["command"];

    if (id && String(id) == robotID) {
        if (command) {
            if (strcmp(command, "adelante") == 0) {
                Serial.println("Mover adelante");
                analogWrite(ENA, 100);
                analogWrite(ENB, 100);
                digitalWrite(IN1, HIGH);
                digitalWrite(IN2, LOW);
                digitalWrite(IN3, HIGH);
                digitalWrite(IN4, LOW);
            } else if (strcmp(command, "atras") == 0) {
                Serial.println("Mover atrás");
                analogWrite(ENA, 100);
                analogWrite(ENB, 100);
                digitalWrite(IN1, LOW);
                digitalWrite(IN2, HIGH);
                digitalWrite(IN3, LOW);
                digitalWrite(IN4, HIGH);
            } else if (strcmp(command, "izquierda") == 0) {
                Serial.println("Girar izquierda");
                analogWrite(ENA, 100);
                analogWrite(ENB, 100);
                digitalWrite(IN1, LOW);
                digitalWrite(IN2, HIGH);
                digitalWrite(IN3, HIGH);
                digitalWrite(IN4, LOW);
            } else if (strcmp(command, "derecha") == 0) {
                Serial.println("Girar derecha");
                analogWrite(ENA, 100);
                analogWrite(ENB, 100);
                digitalWrite(IN1, HIGH);
                digitalWrite(IN2, LOW);
                digitalWrite(IN3, LOW);
                digitalWrite(IN4, HIGH);
            } else if (strcmp(command, "stop") == 0) {
                Serial.println("Detener");
                digitalWrite(IN1, LOW);
                digitalWrite(IN2, LOW);
                digitalWrite(IN3, LOW);
                digitalWrite(IN4, LOW);
            } else {
                Serial.println("Comando no reconocido.");
            }
        }
    } else {
        Serial.println("Mensaje no dirigido a este robot.");
    }
    break;
}

    }
}

void setup() {
    Serial.begin(115200);

    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);

    // Conexión WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Conectando a WiFi...");
    }
    Serial.println("Conectado a WiFi");

    // Conectar al servidor WebSocket
    webSocket.beginSSL(serverIp, serverPort, "/");
    webSocket.onEvent(webSocketEvent);
}

void loop() {
    webSocket.loop(); // Mantener conexión activa
}

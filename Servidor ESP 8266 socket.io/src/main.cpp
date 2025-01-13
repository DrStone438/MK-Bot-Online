#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>
/*Wifi Casa
const char* ssid = "Casa Bello";
const char* password = "N66k8nbe";
*/
//Wifi Trabajo
//const char* ssid = "Grupo EPM";
//const char* password = "Soporte632";
//Wifi Telefono
const char* ssid = "S21 de Joaquin";
const char* password = "pdxm5512";

// IP de Casa
//const char* serverIp = "192.168.100.13"; // Cambia por la IP del servidor
// IP de Trabajo
//const char* serverIp = "201.236.162.98";

// Ip del telefono
const char* serverIp = "186.11.80.196";
const int serverPort = 3000;



WebSocketsClient webSocket;

void webSocketEvent(WStype_t type, uint8_t* payload, size_t length) {
    switch (type) {
        case WStype_DISCONNECTED:
            Serial.println("Desconectado del servidor");
            break;
        case WStype_CONNECTED:
            Serial.println("Conectado al servidor WebSocket");
            webSocket.sendTXT("ESP conectado");
            break;
        case WStype_TEXT:
            Serial.println("Mensaje recibido desde WebSocket");
            Serial.printf("Payload recibido: %s\n", (char*)payload);
            String command = String((char*)payload);
            Serial.println("Comando recibido: " + command);


            // Ejecutar el comando
            if ((char*)payload == "adelante") {
                // Lógica para mover adelante
                digitalWrite(D1, HIGH);
                Serial.println("adelante");
            } else if (command == "atras") {
                // Lógica para mover atrás
            } else if (command == "izquierda") {
                // Lógica para girar izquierda
            } else if (command == "derecha") {
                // Lógica para girar derecha
            } else if (command == "stop") {
                // Lógica para detener
                digitalWrite(D1, LOW);
            }
            Serial.println("Mensaje recibido desde WebSocket");
            Serial.printf("Payload recibido 2: %s\n", (char*)payload);
            break;
    }
}

void setup() {
    Serial.begin(115200);

    // Conexión WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Conectando a WiFi...");
    }
    Serial.println("Conectado a WiFi");

    // Conectar al servidor WebSocket
    webSocket.begin(serverIp, serverPort, "/");
    webSocket.onEvent(webSocketEvent);

    pinMode(D1, OUTPUT);
}

void loop() {
    webSocket.loop();  // Mantener conexión activa
}

#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>
#include <arduino.h>

// Wifi Casa
//const char *ssid = "Casa Bello";
//const char *password = "N66k8nbe";

// Wifi Trabajo
 const char* ssid = "Grupo EPM";
 const char* password = "Soporte632";

// IP de Casa
//const char *serverIp = "192.168.100.13"; // Cambia por la IP del servidor
// IP de Trabajo
 const char* serverIp = "201.236.162.98";

const int serverPort = 3000;
WebSocketsClient webSocket;

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length)
{
    switch (type)
    {
    case WStype_DISCONNECTED:
        Serial.println("Desconectado del servidor");
        break;
    case WStype_CONNECTED:
        Serial.println("Conectado al servidor WebSocket");
        webSocket.sendTXT("ESP conectado");
        break;
    case WStype_TEXT:
        Serial.println("Mensaje recibido desde WebSocket");
        Serial.printf("Payload recibido: %s\n", (char *)payload);
        String command = String((char *)payload);
        Serial.println("Comando recibido: " + command);

        // Ejecutar el comando
        if (command.equals("adelante"))
        {
            // Lógica para mover adelante
            digitalWrite(D1, HIGH);
            Serial.println("adelante");
        }
        else if (command.equals("atras"))
        {
            // Lógica para mover atrás
            Serial.println("atras");
        }
        else if (command.equals("izquierda"))
        {
            // Lógica para girar izquierda
            Serial.println("izquierda");
        }
        else if (command.equals("derecha"))
        {
            // Lógica para girar derecha
            Serial.println("derecha");
        }
        else if (command.equals("stop"))
        {
            // Lógica para detener
            digitalWrite(D1, LOW);
            Serial.println("stop");
        }
        break;
    }
}

void setup()
{
    Serial.begin(115200);

    // Conexión WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.println("Conectando a WiFi...");
    }
    Serial.println("Conectado a WiFi");

    // Conectar al servidor WebSocket
    webSocket.begin(serverIp, serverPort, "/");
    webSocket.onEvent(webSocketEvent);

    pinMode(D1, OUTPUT);
}

void loop()
{
    webSocket.loop(); // Mantener conexión activa
}
#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>
#include <arduino.h>

//const char *ssid = "Casa Bello";// Cambia por el nombre de tu red
//const char *password = "N66k8nbe";// Cambia por la contraseña de tu red

// Wifi Trabajo
const char* ssid = "ZTE-2.4G";
const char* password = "math9793";

// IP del Servidor 
const char *serverIp = "mk-bot-online.onrender.com"; // Cambia por la IP del servidor

const int serverPort = 443;
WebSocketsClient webSocket;



void webSocketEvent(WStype_t type, uint8_t *payload, size_t length)
{
    switch (type)
    {
    case WStype_DISCONNECTED:
        Serial.println("Desconectado del servidor");
        digitalWrite(LED_BUILTIN, HIGH);
        delay(500);
        digitalWrite(LED_BUILTIN, LOW);
        delay(500);
        break;
    case WStype_CONNECTED:
        Serial.println("Conectado al servidor WebSocket");
        delay(500);// deja un ligero retraso para que el servidor pueda responder
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
            Serial.println("Comando: adelante detectado");
            digitalWrite(D1, HIGH);
            Serial.println("adelante");
            webSocket.sendTXT("Comando ejecutado: adelante");// manda un mensaje al servidor de que se ejecuto el comando
        }
        else if (command.equals("atras"))
        {
            // Lógica para mover atrás
            Serial.println("atras");
            digitalWrite(D2,HIGH);
        }
        else if (command.equals("izquierda"))
        {
            // Lógica para girar izquierda
            Serial.println("izquierda");
            digitalWrite(D3,HIGH);
        }
        else if (command.equals("derecha"))
        {
            // Lógica para girar derecha
            Serial.println("derecha");
            digitalWrite(D4,HIGH);
        }
        else if (command.equals("stop"))
        {
            // Lógica para detener
            digitalWrite(D1, LOW);
            digitalWrite(D2, LOW);
            digitalWrite(D3, LOW);
            digitalWrite(D4, LOW);
            Serial.println("stop");
        }
        else
        {
            Serial.println("Comando no reconocido: " + command);
        }
        break;
    }
}

void setup()
{
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(D2, OUTPUT);
    pinMode(D3, OUTPUT);
    pinMode(D4, OUTPUT);

    // Conexión WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.println("Conectando a WiFi...");
        digitalWrite(LED_BUILTIN, LOW);
        delay(500);
        digitalWrite(LED_BUILTIN, HIGH);
    }
    Serial.println("Conectado a WiFi");

    // Conectar al servidor WebSocket
    webSocket.beginSSL(serverIp, serverPort, "/");
    webSocket.onEvent(webSocketEvent);

    pinMode(D1, OUTPUT);
}

void loop()
{
    webSocket.loop(); // Mantener conexión activa
}
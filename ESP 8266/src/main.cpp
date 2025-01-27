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

// Motor A - Derecho
int ENA = D6;  // ENA conectado al pin digital 10
int IN1 = D1;   // IN1 conectado al pin digital 9
int IN2 = D2;   // IN2 conectado al pin digital 8

// Motor B - Izquierdo
int IN3 = D3;   // IN3 conectado al pin digital 7
int IN4 = D4;   // IN4 conectado al pin digital 6
int ENB = D5;   // ENB conectado al pin digital 5



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
            Serial.println("adelante");
            webSocket.sendTXT("Comando ejecutado: adelante");// manda un mensaje al servidor de que se ejecuto el comando
            analogWrite(ENA, 100);
            analogWrite(ENB, 100);
            digitalWrite(IN1, HIGH);
            digitalWrite(IN2, LOW);
            digitalWrite(IN3, HIGH);
            digitalWrite(IN4, LOW);
        }
        else if (command.equals("atras"))
        {
            // Lógica para mover atrás
            Serial.println("atras");
            analogWrite(ENA, 100);
            analogWrite(ENB, 100);
            digitalWrite(IN1, LOW);
            digitalWrite(IN2, HIGH);
            digitalWrite(IN3, LOW);
            digitalWrite(IN4, HIGH);

        }
        else if (command.equals("izquierda"))
        {
            // Lógica para girar izquierda
            Serial.println("izquierda");
            analogWrite(ENA, 100);
            analogWrite(ENB, 100);
            digitalWrite(IN1, LOW);
            digitalWrite(IN2, HIGH);
            digitalWrite(IN3, HIGH);
            digitalWrite(IN4, LOW);
        }
        else if (command.equals("derecha"))
        {
            // Lógica para girar derecha
            Serial.println("derecha");
            analogWrite(ENA, 100);
            analogWrite(ENB, 100);
            digitalWrite(IN1, HIGH);
            digitalWrite(IN2, LOW);
            digitalWrite(IN3, LOW);
            digitalWrite(IN4, HIGH);
        }
        else if (command.equals("stop"))
        {
            // Lógica para detener
            digitalWrite(IN1, LOW);
            digitalWrite(IN2, LOW);
            digitalWrite(IN3, LOW);
            digitalWrite(IN4, LOW);
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
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
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
    
}

void loop()
{
    webSocket.loop(); // Mantener conexión activa
}
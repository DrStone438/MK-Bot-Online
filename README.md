# MK-BOT: Control Remoto de Robot mediante WebSockets

## Descripción

Este proyecto permite controlar un robot mediante una página web a través de WebSockets. Se compone de tres partes principales:

1. **ESP8266**: Se conecta a una red WiFi y recibe comandos desde un servidor WebSocket para controlar motores.

2. **Servidor WebSocket (Node.js)**: Gestiona la comunicación entre la página web y el ESP32.

3. **Página Web**: Proporciona una interfaz de usuario para enviar comandos al robot.

## Tecnologías Utilizadas

- **ESP8266** con Arduino

- **WiFi y WebSockets** para la comunicación en tiempo real

- **Node.js y Express** para el servidor WebSocket

- **HTML, CSS y Bootstrap** para la página web

## Requisitos

### Hardware

- ESP8266

- Módulo de motores con puente H

- Motores y chasis del robot

### Software

- Arduino IDE con librerías:

  - ESP8266WiFi.h

  - WebSocketsClient.h

  - ArduinoJson.h

- Node.js con dependencias:

  - express

  - ws

  - cors

## Instalación y Configuración

### 1. Configurar ESP8266

- Editar el código en ESP8266.ino:

  - Configurar ssid y password para la red WiFi.

  - Modificar serverIp con la dirección del servidor WebSocket.

- Cargar el código en el ESP8266 usando Arduino IDE.

### 2. Configurar el Servidor WebSocket

- Instalar dependencias:

  ``
npm install express ws cors``

- Iniciar el servidor:

   `node server.js`

- El servidor se ejecutará en http://<IP_DEL_SERVIDOR>:3000

### 3. Ejecutar la Página Web

- Abrir index.html en un navegador.

- Seleccionar el robot y enviar comandos con los botones.

## Uso

1. Encender el ESP8266 y conectarlo a WiFi.

2. Iniciar el servidor Node.js.

3. Abrir la página web y seleccionar el robot a controlar.

4. Usar los botones para enviar comandos al robot.

## Comandos Soportados

| Comando | Acción            |
|---------|-------------------|
|adelante |Avanza             |
|atras    |Retrocede          |
|izquierda|Gira a la izquierda|
|derecha  |Gira a la derecha  |
|stop     |Se detiene         |

## Mejoras Futuras

- Agregar control de velocidad

- Integración con sensores para evitar obstáculos

- Control desde aplicación móvil

Autores

Joaquin Bello - Desarrollador

Licencia

Este proyecto está bajo la licencia MIT.


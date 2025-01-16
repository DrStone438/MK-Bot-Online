const WebSocket = require('ws');
const express = require('express');
const cors = require('cors');

const app = express();
const PORT = 3000;

// Configurar Express
app.use(cors());
app.use(express.static('public')); // Carpeta para la página web

// Servidor HTTP para servir la página web
const server = app.listen(PORT, () => {
    console.log(`Servidor corriendo en http://localhost:${PORT}`);
});

// Servidor WebSocket
const wss = new WebSocket.Server({ server });

wss.on('connection', (ws) => {
    console.log('Cliente conectado');
    ws.send('prueba directa'); // Envía un mensaje de prueba

    // Escuchar mensajes del cliente
    ws.on('message', (message) => {
        console.log(`Comando recibido: ${message}`);
        
        // Reenviar el mensaje a todos los clientes conectados
        wss.clients.forEach((client) => {
            if (client.readyState === WebSocket.OPEN) {
                console.log(`Reenviando Comando: ${message}`);
                //client.send(message);

                if (message == 'adelante'){
                    client.send('adelante');
                }
                else if (message == 'atras'){
                    client.send('atras');
                }
                else if (message == 'derecha'){
                    client.send('derecha');
                }
                else if (message == 'izquierda'){
                    client.send('izquierda');
                }
                else if (message == 'stop'){
                    client.send('stop');
                }
            }
        });
    });

    ws.on('close', () => {
        console.log('Cliente desconectado');
    });
});

// Keep-alive para mantener conexiones
setInterval(() => {
    wss.clients.forEach((client) => {
        if (client.readyState === WebSocket.OPEN) {
            client.send('Sigue conectado');
        }
    });
}, 30000); // Cada 30 segundos
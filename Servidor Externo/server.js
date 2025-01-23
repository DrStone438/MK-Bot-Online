const WebSocket = require('ws');
const express = require('express');
const cors = require('cors');

const app = express();
const PORT = 3000;

// Configurar Express
app.use(cors());
app.use(express.static('public')); // Carpeta para la página web

const os = require('os');

// Obtener la dirección IP local del servidor
const getLocalIP = () => {
    const interfaces = os.networkInterfaces();
    for (const name in interfaces) {
        for (const iface of interfaces[name]) {
            if (iface.family === 'IPv4' && !iface.internal) {
                return iface.address;
            }
        }
    }
    return 'localhost';
};

const localIP = getLocalIP();
const server = app.listen(PORT, () => {
    console.log(`Servidor corriendo en http://${localIP}:${PORT}`);
});

// Servidor WebSocket
const wss = new WebSocket.Server({ server });

// Mapa para asociar robots y conexiones
const robotSessions = new Map();

wss.on('connection', (ws) => {
    console.log('Cliente conectado');

    let clientType = null; // Tipo del cliente: "robot" o "web"
    let robotId = null; // Identificador del robot asociado

    // Escuchar mensajes del cliente
    ws.on('message', (message) => {
        console.log(`Mensaje recibido: ${message}`);
        const parts = message.split(':');

        if (parts[0] === 'register') {
            // Registro de clientes
            clientType = parts[1]; // "robot" o "web"
            robotId = parts[2];

            if (clientType === 'robot') {
                // Registrar un robot
                robotSessions.set(robotId, { robot: ws, web: null });
                console.log(`Robot ${robotId} registrado`);
                ws.send(`registered:${robotId}`);
            } else if (clientType === 'web') {
                // Vincular página web al robot
                if (robotSessions.has(robotId)) {
                    robotSessions.get(robotId).web = ws;
                    console.log(`Página web vinculada a robot ${robotId}`);
                    ws.send(`linked:${robotId}`);
                } else {
                    ws.send(`error:Robot ${robotId} no encontrado`);
                }
            }
        } else if (parts[0] === 'command' && clientType === 'web') {
            // Comando desde la página web
            const command = parts[1];
            if (robotSessions.has(robotId)) {
                const robotConnection = robotSessions.get(robotId).robot;
                if (robotConnection && robotConnection.readyState === WebSocket.OPEN) {
                    robotConnection.send(`command:${command}`);
                    console.log(`Comando enviado a robot ${robotId}: ${command}`);
                } else {
                    ws.send(`error:Robot ${robotId} no está conectado`);
                }
            } else {
                ws.send(`error:Robot ${robotId} no registrado`);
            }
        }
    });

    ws.on('close', () => {
        if (clientType === 'robot' && robotId) {
            robotSessions.delete(robotId);
            console.log(`Robot ${robotId} desconectado`);
        } else if (clientType === 'web' && robotId) {
            const session = robotSessions.get(robotId);
            if (session) {
                session.web = null;
                console.log(`Página web desvinculada de robot ${robotId}`);
            }
        }
    });
});

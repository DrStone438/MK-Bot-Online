const WebSocket = require('ws');
const express = require('express');
const cors = require('cors');

const app = express();
const PORT = process.env.PORT || 3000; // Puerto del servidor

// Configuración de Express
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

// Mapa para gestionar las sesiones de robots y webs asociadas
const robotSessions = new Map();

wss.on('connection', (ws) => {
    console.log('Cliente conectado');

    let clientType = null; // Tipo de cliente: "robot" o "web"
    let robotId = null; // Identificador único del robot asociado

    // Escuchar mensajes del cliente
    ws.on('message', (message) => {
        console.log(`Mensaje recibido: ${message}`);
        const parts = message.split(':');

        if (parts[0] === 'register') {
            // Registrar cliente
            clientType = parts[1]; // "robot" o "web"
            robotId = parts[2];

            if (clientType === 'robot') {
                if (!robotSessions.has(robotId)) {
                    // Registrar nuevo robot
                    robotSessions.set(robotId, { robot: ws, web: null });
                    console.log(`Robot ${robotId} registrado`);
                    ws.send(`registered:${robotId}`);
                } else {
                    // Robot ya registrado
                    ws.send(`error:Robot ${robotId} ya está registrado`);
                }
            } else if (clientType === 'web') {
                if (robotSessions.has(robotId)) {
                    // Vincular página web a un robot existente
                    const session = robotSessions.get(robotId);
                    if (session.web) {
                        ws.send(`error:Ya hay una página web vinculada a robot ${robotId}`);
                    } else {
                        session.web = ws;
                        console.log(`Página web vinculada a robot ${robotId}`);
                        ws.send(`linked:${robotId}`);
                    }
                } else {
                    ws.send(`error:Robot ${robotId} no encontrado`);
                }
            }
        } else if (parts[0] === 'command' && clientType === 'web') {
            // Enviar comando desde la página web al robot
            const command = parts[1];
            if (robotSessions.has(robotId)) {
                const session = robotSessions.get(robotId);
                if (session.robot && session.robot.readyState === WebSocket.OPEN) {
                    session.robot.send(`command:${command}`);
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
            // Eliminar sesión del robot al desconectarse
            const session = robotSessions.get(robotId);
            if (session) {
                if (session.web) {
                    session.web.send(`error:Robot ${robotId} se desconectó`);
                }
                robotSessions.delete(robotId);
                console.log(`Robot ${robotId} desconectado`);
            }
        } else if (clientType === 'web' && robotId) {
            // Desvincular página web al desconectarse
            const session = robotSessions.get(robotId);
            if (session) {
                session.web = null;
                console.log(`Página web desvinculada de robot ${robotId}`);
            }
        }
    });
});

const WebSocket = require('ws');
const express = require('express');
const cors = require('cors');

const app = express();
const PORT = process.env.PORT || 3000; // Puerto del servidor

// Configurar Express
app.use(cors());
app.use(express.static('public')); // Carpeta para la página web

const os = require('os'); // Importar módulo para obtener la IP

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

// Mapeo de ID a conexiones
const sessions = {};

wss.on('connection', (ws) => {
    console.log('Cliente conectado');

    // Escuchar mensajes del cliente
    ws.on('message', (message) => {
        if (typeof message === 'string') {
            try {
                const data = JSON.parse(message); // Convierte el mensaje a un objeto JSON

                if (data.clase === 'robot' && data.ID) {
                    // Registrar el ESP con su ID
                    sessions[data.ID] = ws;
                    console.log(`Robot registrado: ${data.ID}`);
                    ws.send(JSON.stringify({ status: 'registrado', ID: data.ID }));
                } else if (data.clase === 'web' && data.ID) {
                    // Conexión desde una página web
                    const robotWs = sessions[data.ID];
                    if (robotWs && robotWs.readyState === WebSocket.OPEN) {
                        ws.send(JSON.stringify({ status: 'conectado', ID: data.ID }));

                        // Redirigir comandos del cliente web al ESP
                        ws.on('message', (commandMessage) => {
                            try {
                                const commandData = JSON.parse(commandMessage);
                                if (commandData.command) {
                                    console.log(`Comando para ${data.ID}: ${commandData.command}`);
                                    robotWs.send(JSON.stringify({ command: commandData.command }));
                                } else {
                                    console.error('Comando no válido');
                                }
                            } catch (err) {
                                console.error('Error al procesar el comando:', err.message);
                            }
                        });
                    } else {
                        ws.send(JSON.stringify({ error: 'Robot no encontrado', ID: data.ID }));
                    }
                } else {
                    ws.send(JSON.stringify({ error: 'Mensaje no válido' }));
                }
            } catch (err) {
                console.error('Error al procesar mensaje:', err.message);
                ws.send(JSON.stringify({ error: 'Formato inválido' }));
            }
        } else {
            // Es un mensaje binario (imagen del ESP32-CAM)
            for (const id in sessions) {
                if (sessions[id].robot === ws && sessions[id].web) {
                    // Reenviar la imagen al cliente web vinculado
                    sessions[id].web.send(message);
                }
            }
        }
    });

    ws.on('close', () => {
        // Eliminar sesiones desconectadas
        for (const id in sessions) {
            if (sessions[id] === ws) {
                console.log(`Robot desconectado: ${id}`);
                delete sessions[id];
                break;
            }
        }
    });
});

// Keep-alive para mantener conexiones
setInterval(() => {
    wss.clients.forEach((client) => {
        if (client.readyState === WebSocket.OPEN) {
            client.send(JSON.stringify({ status: 'ping' }));
        }
    });
}, 30000); // Cada 30 segundos

const WebSocket = require('ws');
const express = require('express');
const cors = require('cors');

const app = express();
const PORT = process.env.PORT || 3000;//Ingrese el puerto de el servidor

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
        if (data.clase === 'web' && data.ID) {
            const robotWs = sessions[data.ID];
            if (robotWs && robotWs.readyState === WebSocket.OPEN) {
                ws.send(JSON.stringify({ status: 'conectado', ID: data.ID }));
                
                // Redirigir comandos del cliente web al ESP
                ws.on('message', (command) => {
                    try {
                        const parsedCommand = JSON.parse(command); // Asegúrate de analizar como JSON
                        if (parsedCommand.ID === data.ID && parsedCommand.command) {
                            console.log(`Comando para ${data.ID}: ${parsedCommand.command}`);
                            robotWs.send(JSON.stringify(parsedCommand)); // Enviar al ESP
                        } else {
                            ws.send(JSON.stringify({ error: 'Formato de comando inválido' }));
                        }
                    } catch (err) {
                        console.error('Error al procesar comando:', err.message);
                        ws.send(JSON.stringify({ error: 'Formato inválido' }));
                    }
                });
            } else {
                ws.send(JSON.stringify({ error: 'Robot no encontrado', ID: data.ID }));
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

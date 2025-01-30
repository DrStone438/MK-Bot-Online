const WebSocket = require('ws');
const express = require('express');
const cors = require('cors');

const app = express();
const PORT = process.env.PORT || 3000;

// Configurar Express
app.use(cors());
app.use(express.static('public'));

const server = app.listen(PORT, () => {
    console.log(`Servidor corriendo en puerto ${PORT}`);
});

// Servidor WebSocket
const wss = new WebSocket.Server({ server });

const sessions = {};

wss.on('connection', (ws) => {
    console.log('Cliente conectado');

    ws.on('message', (message) => {
        try {
            const data = JSON.parse(message);

            if (data.clase === 'robot' && data.ID) {
                sessions[data.ID] = { robot: ws };
                console.log(`Robot registrado: ${data.ID}`);
                ws.send(JSON.stringify({ status: 'registrado', ID: data.ID }));
            } else if (data.clase === 'web' && data.ID) {
                if (sessions[data.ID] && sessions[data.ID].robot) {
                    ws.send(JSON.stringify({ status: 'conectado', ID: data.ID }));
                    sessions[data.ID].web = ws;

                    ws.on('message', (commandMessage) => {
                        try {
                            const commandData = JSON.parse(commandMessage);
                            if (commandData.command) {
                                console.log(`Comando para ${data.ID}: ${commandData.command}`);
                                sessions[data.ID].robot.send(JSON.stringify({ command: commandData.command }));
                            }
                        } catch (err) {
                            console.error('Error al procesar el comando:', err.message);
                        }
                    });
                } else {
                    ws.send(JSON.stringify({ error: 'Robot no encontrado', ID: data.ID }));
                }
            }
        } catch (err) {
            console.error('Error al procesar mensaje:', err.message);
            ws.send(JSON.stringify({ error: 'Formato inválido' }));
        }
    });

    ws.on('close', () => {
        for (const id in sessions) {
            if (sessions[id].robot === ws) {
                console.log(`Robot desconectado: ${id}`);
                delete sessions[id];
            }
        }
    });
});

// Keep-alive
setInterval(() => {
    wss.clients.forEach((client) => {
        if (client.readyState === WebSocket.OPEN) {
            client.send(JSON.stringify({ status: 'ping' }));
        }
    });
}, 30000);

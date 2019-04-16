const http = require('http');

const WebSocket = require('ws');

module.exports = function(app) {
  const server = http.createServer(app);
  const wss = new WebSocket.Server({ server, port: WEBSOCKET_PORT });

  wss.on('connection', function connection(ws) {
    ws.on('message', function incoming(message) {
      console.log(`received: ${message}`);
    });
  });

  return {
    broadcast: data => {
      wss.clients.forEach(function each(client) {
        if (client.readyState === WebSocket.OPEN) {
          client.send(data);
        }
      });
    },
  };
};

const http = require('http');

const WebSocket = require('ws');

const environment = require('../lib/environment');

module.exports = function(app) {
  const server = http.createServer(app);
  const wss = new WebSocket.Server({ server, port: WEBSOCKET_PORT });

  wss.on('connection', function connection(ws) {
    let currentTiime = 1;
    function sendTimestamp() {
      if (ws.readyState === WebSocket.OPEN) {
        ws.send(`${currentTiime}`);
        currentTiime += 1;

        if (environment.isDevelopment && currentTiime > 15) {
          currentTiime = 1;
        }
      }
      setTimeout(sendTimestamp, 100);
    }

    sendTimestamp();

    ws.on('message', function incoming(message) {
      console.log(`received: ${message}`);
    });
  });
};

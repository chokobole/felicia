const path = require('path');
const http = require('http');

const express = require('express');
const morgan = require('morgan');
const WebSocket = require('ws');

const devMiddleware = require('./lib/dev-middleware');
const environment = require('./lib/environment');
const packagejson = require('../package.json');

const app = express();

if (environment.isDevelopment) {
  app.use(morgan('dev'));
  devMiddleware(app);
  app.use(express.static(path.resolve(__dirname, '..', '..', 'frontend', 'dist')));
} else {
  app.use(express.static('dist'));
}

const server = http.createServer();

const wss = new WebSocket.Server({ server, port: 3001 });

wss.on('connection', function connection(ws) {
  let currentTiime = 0;
  function sendTimestamp() {
    if (ws.readyState === WebSocket.OPEN) {
      ws.send(`${currentTiime}`);
      currentTiime += 1;
    }
    setTimeout(sendTimestamp, 100);
  }

  sendTimestamp();

  ws.on('message', function incoming(message) {
    console.log(`received: ${message}`);
  });
});

const PORT = process.env.PORT || 3000;
app.listen(PORT, () => console.log(`Running FeliciaViz-${packagejson.version} on ${PORT}`));

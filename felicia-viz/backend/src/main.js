const express = require('express');
const morgan = require('morgan');

const feliciaJs = require('felicia_js.node');
const devMiddleware = require('./lib/dev-middleware');
const environment = require('./lib/environment');
const packagejson = require('../package.json');
const websocketMiddleware = require('./websocket');

const app = express();

if (environment.isDevelopment) {
  app.use(morgan('dev'));
  devMiddleware(app);
} else {
  app.use(morgan('common'));
  app.use(express.static('dist'));
}

app.use(express.static('static'));

const websocket = websocketMiddleware(app);

app.listen(HTTP_PORT, () =>
  console.log(`Running FeliciaViz-${packagejson.version} on ${HTTP_PORT}`)
);

feliciaJs.MasterProxy.setBackground();

const s = feliciaJs.MasterProxy.start();
if (!s.ok()) {
  process.exit(1);
}

feliciaJs.MasterProxy.requestRegisterDynamicSubscribingNode(
  function(topic, message) {
    console.log(`[TOPIC]: ${topic}`);
    if (message.type === 'felicia.CameraMessage') {
      const { timestamp, data } = message.message;
      websocket.broadcast(
        JSON.stringify({
          currentTime: timestamp,
          frame: {
            length: data.byteLength,
            width: 640,
            height: 480,
            data: new Uint8Array(data),
          },
        })
      );
    }
  },
  function(topic, status) {
    console.log(`[TOPIC]: ${topic}`);
    console.error(status.errorMessage());
  },
  {
    period: 100,
  }
);

feliciaJs.MasterProxy.run();

const express = require('express');
const morgan = require('morgan');

const feliciaJs = require('felicia_js.node');
const devMiddleware = require('./lib/dev-middleware');
const environment = require('./lib/environment');
const packagejson = require('../package.json');
const websocketMiddleWare = require('./websocket');

const app = express();

if (environment.isDevelopment) {
  app.use(morgan('dev'));
  devMiddleware(app);
} else {
  app.use(morgan('common'));
  app.use(express.static('dist'));
}

app.use(express.static('static'));

websocketMiddleWare(app);

app.listen(HTTP_PORT, () =>
  console.log(`Running FeliciaViz-${packagejson.version} on ${HTTP_PORT}`)
);

feliciaJs.MasterProxy.setBackground();

const s = feliciaJs.MasterProxy.start();

feliciaJs.MasterProxy.requestRegisterDynamicSubscribingNode(function (topic, message) {
  console.log(`[TOPIC]: ${topic}`);
  console.log(message);
}, function (topic, s) {
  console.log(`[TOPIC]: ${topic}`);
  console.error(s.error_message());
});

feliciaJs.MasterProxy.run();

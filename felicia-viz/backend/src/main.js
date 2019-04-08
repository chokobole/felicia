const express = require('express');
const morgan = require('morgan');

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

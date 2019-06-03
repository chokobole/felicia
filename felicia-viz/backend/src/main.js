import path from 'path';

import express from 'express';
import morgan from 'morgan';

import feliciaBinding from 'felicia-binding';
import devMiddleware from 'lib/dev-middleware';
import { isDevelopment } from 'lib/environment';
import packagejson from '../package.json';

const app = express();

if (isDevelopment) {
  app.use(morgan('dev'));
  devMiddleware(app);
} else {
  app.use(morgan('common'));
  app.use(express.static('dist'));
}

express.static.mime.define({ 'application/wasm': ['wasm'] });
app.use(express.static(path.resolve('../wasm')));

app.listen(HTTP_PORT, () =>
  console.log(`Running FeliciaViz-${packagejson.version} on ${HTTP_PORT}`)
);

feliciaBinding();

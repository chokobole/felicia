// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import express from 'express';
import feliciaBinding from 'felicia-binding';
import devMiddleware from 'lib/dev-middleware';
import { isDevelopment } from 'lib/environment';
import morgan from 'morgan';
import path from 'path';
import { HTTP_PORT } from 'typings/settings';
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

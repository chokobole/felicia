// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

const { resolve } = require('path');

const nodeExternals = require('webpack-node-externals');
const StartServerPlugin = require('start-server-webpack-plugin');

const commonConfig = require('./webpack.config.common.js');

const ROOT_PATH = resolve(__dirname, '..');

const CONFIG = {
  entry: {
    server: resolve('src/main.ts'),
  },

  target: 'node',

  externals: [nodeExternals()],

  output: {
    path: resolve('dist'),
    publicPath: '/',
    filename: '[name].js',
  },
};

module.exports = env => {
  const config = Object.assign(commonConfig(env), CONFIG);

  // Should move under development once published
  let feliciaJsNodePath = resolve(ROOT_PATH, '../felicia_js.node');
  config.resolve.alias = Object.assign(config.resolve.alias, {
    'felicia_js.node': feliciaJsNodePath,
  });

  config.resolve.modules = config.resolve.modules.concat(resolve('src'), resolve('node_modules'));

  config.module.rules = config.module.rules.concat({
    test: /\.node$/,
    use: 'node-loader',
  });

  if (env.production) {
    // production
  } else {
    // development
    config.watch = true;

    config.plugins = config.plugins.concat([
      new StartServerPlugin({
        name: 'server.js',
        nodeArgs: ['--inspect=7777'],
      }),
    ]);
  }

  return config;
};

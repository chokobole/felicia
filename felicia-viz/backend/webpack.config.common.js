// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

const { resolve } = require('path');

const ForkTsCheckerWebpackPlugin = require('fork-ts-checker-webpack-plugin');
const webpack = require('webpack');

const CONFIG = {
  module: {
    rules: [
      {
        test: /webworker\.(ts|js)$/,
        use: [
          {
            loader: 'worker-loader',
          },
        ],
      },
      {
        test: /\.(ts|tsx|js)$/,
        exclude: /node_modules/,
        use: [
          {
            loader: 'babel-loader',
            options: {
              rootMode: 'upward',
            },
          },
        ],
      },
    ],
  },

  plugins: [
    new webpack.DefinePlugin({
      HEARTBEAT_INTERVAL: 30000,
      HTTP_PORT: 3000,
      WEBSOCKET_PORT: 3001,
      SERVER_ADDRESS: JSON.stringify('localhost'),
      FELICIA_ROOT: JSON.stringify(resolve('../..')),
    }),
  ],

  resolve: {
    extensions: ['.ts', '.tsx', '.js', '.json'],
    modules: [resolve('..')],
  },
};

module.exports = env => {
  const config = Object.assign({}, CONFIG);

  // Should move under development once published
  config.resolve.alias = {
    '@felicia-viz/communication': resolve('../modules/communication/src'),
    '@felicia-viz/proto': resolve('../modules/proto/src'),
  };

  config.resolve.modules = config.resolve.modules.concat(resolve('../modules/proto/node_modules'));

  if (env.production) {
    // production
    Object.assign(config, {
      mode: 'production',
    });
  } else {
    // development
    Object.assign(config, {
      mode: 'development',

      devtool: 'source-map',
    });

    config.plugins.push(
      new ForkTsCheckerWebpackPlugin({
        tsconfig: resolve('../tsconfig.json'),
      })
    );

    config.module.rules = config.module.rules.concat({
      enforce: 'pre',
      test: /\.js$/,
      use: ['source-map-loader'],
    });
  }

  return config;
};

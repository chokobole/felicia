// Copyright (c) 2019 The Felicia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

const { resolve } = require('path');

const webpack = require('webpack');
const HtmlWebpackPlugin = require('html-webpack-plugin');

const commonConfig = require('./webpack.config.common.js');

module.exports = env => {
  const config = commonConfig(env);
  const rootPath = '..';

  Object.assign(config, {
    entry: {
      app: resolve(rootPath, 'frontend/src/main.tsx'),
    },

    output: {
      path: resolve(rootPath, 'backend/dist'),
      publicPath: '/',
      filename: 'bundle.js',
      globalObject: 'this',
    },
  });

  config.resolve.alias = Object.assign(config.resolve.alias, {
    '@felicia-viz/deeplearning': resolve(rootPath, 'modules/deeplearning/src'),
  });

  config.resolve.modules = config.resolve.modules.concat(
    resolve(rootPath, 'frontend/src'),
    resolve(rootPath, 'frontend/node_modules')
  );

  config.module.rules = config.module.rules.concat(
    {
      test: /\.s?css$/,
      use: [
        {
          loader: 'style-loader',
        },
        {
          loader: 'css-loader',
        },
        {
          loader: 'sass-loader',
          options: {
            includePaths: ['./node_modules', '.'],
          },
        },
      ],
    },
    {
      test: /\.(svg|ico|gif|jpe?g|png|eot|svg|ttf|woff2?)$/,
      loader: 'file-loader?name=[name].[ext]',
    }
  );

  // Should move under development once published
  config.resolve.alias = Object.assign(config.resolve.alias, {
    '@felicia-viz/ui': resolve(rootPath, 'modules/ui/src'),
  });

  config.resolve.modules = config.resolve.modules.concat(resolve('../modules/ui/node_modules'));

  if (env.production) {
    // production
    config.plugins.unshift(
      new HtmlWebpackPlugin({
        template: resolve(rootPath, 'frontend/dist/index.html'),
        inject: true,
      })
    );
  } else {
    // development
    config.resolve.alias = Object.assign(config.resolve.alias, {
      'react-dom': '@hot-loader/react-dom',
    });

    config.plugins.unshift(
      new HtmlWebpackPlugin({
        template: resolve(rootPath, 'frontend/dist/index.html'),
        inject: true,
      }),
      new webpack.optimize.OccurrenceOrderPlugin(),
      new webpack.HotModuleReplacementPlugin(),
      new webpack.NoEmitOnErrorsPlugin()
    );
  }

  return config;
};

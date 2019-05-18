const { resolve } = require('path');

const webpack = require('webpack');
const HtmlWebpackPlugin = require('html-webpack-plugin');

const commonConfig = require('./webpack.config.common.js');

module.exports = env => {
  const config = commonConfig(env);

  let rootPath;
  if (env.build) {
    rootPath = '..';
  } else {
    // eslint-disable-next-line prefer-destructuring
    rootPath = env.rootPath;
  }

  Object.assign(config, {
    entry: {
      app: resolve(rootPath, 'frontend/src/main.js'),
    },

    output: {
      path: resolve(rootPath, 'backend/dist'),
      publicPath: '/',
      filename: 'bundle.js',
      globalObject: 'this',
    },
  });

  config.resolve.modules = config.resolve.modules.concat(
    resolve(rootPath, 'frontend/src'),
    resolve(rootPath, 'frontend/node_modules')
  );

  config.module.rules = config.module.rules.concat(
    {
      test: /\.js$/,
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
    {
      test: /webworker\.js$/,
      use: [
        {
          loader: 'worker-loader',
        },
      ],
    },
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
  config.resolve.alias = {
    '@felicia-viz/ui': resolve(rootPath, 'modules/ui/src'),
    '@felicia-viz/communication': resolve(rootPath, 'modules/communication/src'),
  };

  if (env.production) {
    // production
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
      new webpack.NoEmitOnErrorsPlugin(),
      new webpack.DefinePlugin({
        SERVER_ADDRESS: JSON.stringify('localhost'),
      })
    );
  }

  return config;
};

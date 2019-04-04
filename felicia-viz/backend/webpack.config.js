const { resolve } = require('path');

const webpack = require('webpack');
const HtmlWebpackPlugin = require('html-webpack-plugin');

const webpackConfigure = require('../webpack.config');

const ROOT_PATH = resolve(__dirname, '..');

module.exports = env => {
  const config = webpackConfigure(env);
  Object.assign(config, {
    entry: {
      app: resolve(ROOT_PATH, 'frontend/src/main.js'),
    },

    output: {
      path: resolve('dist'),
      publicPath: '/',
      filename: 'bundle.js',
    },
  });

  if (env.prod) {
    // production
  } else {
    // development
    Object.assign(config, {
      watch: true,

      plugins: [
        new HtmlWebpackPlugin({
          template: resolve(ROOT_PATH, 'frontend/dist/index.html'),
          inject: true,
        }),
        new webpack.optimize.OccurrenceOrderPlugin(),
        new webpack.HotModuleReplacementPlugin(),
        new webpack.NoEmitOnErrorsPlugin(),
      ],
    });
  }

  return config;
};

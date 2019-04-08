const { resolve } = require('path');

const webpack = require('webpack');
const nodeExternals = require('webpack-node-externals');
const StartServerPlugin = require('start-server-webpack-plugin');

const CONFIG = {
  entry: {
    server: resolve('src/main.js'),
  },

  target: 'node',

  externals: [nodeExternals()],

  plugins: [
    new webpack.DefinePlugin({
      HTTP_PORT: 3000,
      WEBSOCKET_PORT: 3001,
    }),
  ],
};

module.exports = env => {
  const config = Object.assign({}, CONFIG);

  Object.assign(config, {
    output: {
      path: resolve('dist'),
      publicPath: '/',
      filename: '[name].js',
    },
  });

  if (env.production) {
    // production
    Object.assign(config, {
      mode: 'production',
    });
  } else {
    // development
    Object.assign(config, {
      mode: 'development',

      watch: true,

      devtool: 'source-map',

      module: {
        rules: [
          {
            enforce: 'pre',
            test: /\.js$/,
            use: ['source-map-loader'],
          },
        ],
      },
    });

    config.plugins = config.plugins.concat([
      new StartServerPlugin({
        name: 'server.js',
        nodeArgs: ['--inspect=7777'],
      }),
    ]);
  }

  return config;
};

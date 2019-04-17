const { resolve } = require('path');

const webpack = require('webpack');
const nodeExternals = require('webpack-node-externals');
const StartServerPlugin = require('start-server-webpack-plugin');

const ROOT_PATH = resolve(__dirname, '..');

const CONFIG = {
  entry: {
    server: resolve('src/main.js'),
  },

  target: 'node',

  externals: [nodeExternals()],

  module: {
    rules: [
      {
        test: /\.node$/,
        use: 'node-loader',
      },
    ],
  },

  plugins: [
    new webpack.DefinePlugin({
      HTTP_PORT: 3000,
      WEBSOCKET_PORT: 3001,
    }),
  ],

  resolve: {
    alias: {
      'felicia_js.node': resolve(ROOT_PATH, '../bazel-bin/felicia/js/felicia_js.node'),
    },
  },
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
    });

    config.module.rules = config.module.rules.concat({
      enforce: 'pre',
      test: /\.js$/,
      use: ['source-map-loader'],
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

const { resolve } = require('path');

const nodeExternals = require('webpack-node-externals');
const StartServerPlugin = require('start-server-webpack-plugin');

const commonConfig = require('./webpack.config.common.js');

const ROOT_PATH = resolve(__dirname, '..');

const CONFIG = {
  entry: {
    server: resolve('src/main.js'),
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

  config.resolve.alias = {
    'felicia_js.node': resolve(ROOT_PATH, '../bazel-bin/felicia/js/felicia_js.node'),
  };

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

const { resolve } = require('path');

const webpack = require('webpack');

const CONFIG = {
  module: {
    rules: [
      {
        test: /webworker\.js$/,
        use: [
          {
            loader: 'worker-loader',
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
      FELICIA_ROOT: JSON.stringify(resolve('../..')),
    }),
  ],

  resolve: {
    modules: [resolve('..')],
  },
};

module.exports = env => {
  const config = Object.assign({}, CONFIG);

  // Should move under development once published
  config.resolve.alias = {
    '@felicia-viz/communication': resolve('../modules/communication/src'),
  };

  // To resolve protobufjs
  config.resolve.modules = config.resolve.modules.concat(
    resolve('../modules/communication/node_modules')
  );

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

    config.module.rules = config.module.rules.concat({
      enforce: 'pre',
      test: /\.js$/,
      use: ['source-map-loader'],
    });
  }

  return config;
};

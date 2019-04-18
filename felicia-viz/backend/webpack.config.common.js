const { resolve } = require('path');

const webpack = require('webpack');

const CONFIG = {
  plugins: [
    new webpack.DefinePlugin({
      HEARTBEAT_INTERVAL: 30000,
      HTTP_PORT: 3000,
      WEBSOCKET_PORT: 3001,
    }),
  ],

  resolve: {
    modules: [resolve('..')],
  },
};

module.exports = env => {
  const config = Object.assign({}, CONFIG);

  if (env.production) {
    // production
    Object.assign(config, {
      mode: 'production',

      module: {
        rules: [],
      },
    });
  } else {
    // development
    Object.assign(config, {
      mode: 'development',

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
  }

  return config;
};

const { resolve } = require('path');

const HtmlWebpackPlugin = require('html-webpack-plugin');

const webpackConfigure = require('../webpack.config');

module.exports = env => {
  const config = webpackConfigure(env);
  Object.assign(config, {
    entry: {
      app: resolve('src/main.js'),
    },

    output: {
      path: resolve('./dist'),
      publicPath: '/',
      filename: 'bundle.js',
    },
  });

  if (env.prod) {
    // production
  } else {
    // development
    Object.assign(config, {
      devServer: {
        hot: true,
        contentBase: [resolve('./dist')],
      },

      plugins: [
        new HtmlWebpackPlugin({
          template: resolve('./dist/index.html'),
          inject: true,
        }),
      ],
    });
  }

  return config;
};

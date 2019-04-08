const { resolve } = require('path');

const webpack = require('webpack');
const HtmlWebpackPlugin = require('html-webpack-plugin');

const CONFIG = {
  module: {
    rules: [
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
        test: /\.(svg|ico|gif|jpe?g|png)$/,
        loader: 'file-loader?name=[name].[ext]',
      },
    ],
  },

  plugins: [
    new webpack.DefinePlugin({
      HTTP_PORT: 3000,
      WEBSOCKET_PORT: 3001,
    }),
  ],
};

module.exports = env => {
  const config = Object.assign({}, CONFIG);
  const { rootPath } = env;

  Object.assign(config, {
    entry: {
      app: resolve(rootPath, 'frontend/src/main.js'),
    },

    output: {
      path: resolve(rootPath, 'dist'),
      publicPath: '/',
      filename: 'bundle.js',
    },

    resolve: {
      modules: [resolve(rootPath, 'frontend/src'), resolve(rootPath, 'frontend/node_modules')],
    },
  });

  if (env.prod) {
    // production
    Object.assign(config, {
      mode: 'production',
    });
  } else {
    // development
    Object.assign(config, {
      mode: 'development',

      devtool: 'source-map',

      watch: true,
    });

    config.module.rules = config.module.rules.concat({
      enforce: 'pre',
      test: /\.js$/,
      use: ['source-map-loader'],
    });

    config.resolve.alias = {
      'react-dom': '@hot-loader/react-dom',
      '@felicia-viz/config': resolve(rootPath, 'modules/config/src'),
      '@felicia-viz/ui': resolve(rootPath, 'modules/ui/src'),
    };

    config.plugins.unshift(
      new HtmlWebpackPlugin({
        template: resolve(rootPath, 'frontend/dist/index.html'),
        inject: true,
      }),
      new webpack.optimize.OccurrenceOrderPlugin(),
      new webpack.HotModuleReplacementPlugin(),
      new webpack.NoEmitOnErrorsPlugin(),
      new webpack.DefinePlugin({
        SERVER_ADDRESS: 'localhost',
      })
    );
  }

  return config;
};

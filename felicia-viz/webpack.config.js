const { resolve } = require('path');

const ROOT_PATH = resolve(__dirname);

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
};

module.exports = (env, plugins) => {
  const config = Object.assign({}, CONFIG);

  config.plugins = plugins;

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

      resolve: {
        modules: [resolve(ROOT_PATH, 'frontend/src'), 'node_modules'],
        alias: {
          'react-dom': '@hot-loader/react-dom',
          '@felicia-viz/config': resolve(ROOT_PATH, 'modules/config/src'),
          '@felicia-viz/ui': resolve(ROOT_PATH, 'modules/ui/src'),
        },
      },
    });

    config.module.rules = config.module.rules.concat({
      enforce: 'pre',
      test: /\.js$/,
      use: ['source-map-loader'],
    });
  }

  return config;
};

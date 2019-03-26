const { resolve } = require('path');

const ROOT_PATH = resolve(__dirname, '..');

const CONFIG = {
  entry: {
    app: resolve('./src/main.js')
  },

  output: {
    path: resolve('./dist'),
    filename: 'bundle.js'
  },

  module: {
    rules: [
      {
        test: /\.js$/,
        exclude: /node_modules/,
        use: [
          {
            loader: 'babel-loader',
            options: {
              rootMode: 'upward'
            }
          }
        ]
      },
      {
        test: /\.s?css$/,
        use: [
          {
            loader: 'style-loader'
          },
          {
            loader: 'css-loader'
          },
          {
            loader: 'sass-loader',
            options: {
              includePaths: ['./node_modules', '.']
            }
          }
        ]
      },
      {
        test: /\.(svg|ico|gif|jpe?g|png)$/,
        loader: 'file-loader?name=[name].[ext]'
      }
    ]
  }
};

module.exports = (env) => {
  const config = Object.assign({}, CONFIG);

  if (env.prod) {
    // production
    Object.assign(config, {
      mode: 'production'
    });
  } else {
    // development
    Object.assign(config, {
      mode: 'development',
      devServer: {
        contentBase: [resolve(__dirname, './dist')]
      },
      devtool: 'source-map'
    });

    config.module.rules = config.module.rules.concat({
      enforce: 'pre',
      test: /\.js$/,
      use: ['source-map-loader']
    });

    config.resolve = {
      alias: {
        'felicia-viz/ui': resolve(ROOT_PATH, './modules/ui/src')
      }
    };
  }

  return config;
};

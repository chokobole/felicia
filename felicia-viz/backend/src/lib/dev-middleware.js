const path = require('path');

/* eslint import/no-extraneous-dependencies: ["error", {"devDependencies": true}] */
const webpack = require('webpack');
const webpackDevMiddleware = require('webpack-dev-middleware');
const webpackHotMiddleware = require('webpack-hot-middleware');

const webpackConfig = require('../../webpack.config.frontend');

module.exports = function(app) {
  const config = webpackConfig({
    dev: true,
    rootPath: path.resolve('..'),
  });
  const compiler = webpack(config);

  app.use(
    webpackDevMiddleware(compiler, {
      noInfo: true,
      publicPath: compiler.publicPath,
    })
  );

  app.use(webpackHotMiddleware(compiler));
};

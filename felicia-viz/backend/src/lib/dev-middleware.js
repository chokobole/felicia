/* eslint import/no-extraneous-dependencies: ["error", {"devDependencies": true}] */
const webpack = require('webpack');
const webpackDevMiddleware = require('webpack-dev-middleware');
const webpackHotMiddleware = require('webpack-hot-middleware');

const webpackConfig = require('../../webpack.config');

module.exports = function(app) {
  const config = webpackConfig({ backend: true });
  const compiler = webpack(config);

  app.use(
    webpackDevMiddleware(compiler, {
      noInfo: true,
      publicPath: compiler.publicPath,
    })
  );

  app.use(webpackHotMiddleware(compiler));
};

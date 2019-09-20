/* eslint import/no-extraneous-dependencies: ["error", {"devDependencies": true}] */
import { Express } from 'express';
import webpack from 'webpack';
import webpackDevMiddleware from 'webpack-dev-middleware';
import webpackHotMiddleware from 'webpack-hot-middleware';
// @ts-ignore
import webpackConfig from '../../webpack.config.frontend';

export default (app: Express) => {
  const config = webpackConfig({
    dev: true,
  });
  const compiler = webpack(config);
  app.use(webpackDevMiddleware(compiler));
  app.use(webpackHotMiddleware(compiler));
};

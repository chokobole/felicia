import path from 'path';

/* eslint import/no-extraneous-dependencies: ["error", {"devDependencies": true}] */
import webpack from 'webpack';
import webpackDevMiddleware from 'webpack-dev-middleware';
import webpackHotMiddleware from 'webpack-hot-middleware';

import webpackConfig from '../../webpack.config.frontend';

export default app => {
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

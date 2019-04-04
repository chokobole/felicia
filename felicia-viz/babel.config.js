const TARGETS = {
  chrome: '60',
  edge: '15',
  firefox: '53',
  ios: '10.3',
  safari: '10.1',
  node: '10',
};

const CONFIG = {
  default: {
    presets: [
      [
        '@babel/preset-env',
        {
          targets: TARGETS,
        },
      ],
      '@babel/preset-react',
    ],
    plugins: [
      [
        '@babel/plugin-proposal-decorators',
        {
          legacy: true,
        },
      ],
      [
        '@babel/plugin-proposal-class-properties',
        {
          loose: true,
        },
      ],
    ],
  },
};

CONFIG.es6 = Object.assign({}, CONFIG.default, {
  presets: [
    [
      '@babel/preset-env',
      {
        targets: TARGETS,
        modules: false,
      },
    ],
    '@babel/preset-react',
  ],
});

CONFIG.esm = Object.assign({}, CONFIG.default, {
  presets: [
    [
      '@babel/preset-env',
      {
        modules: false,
      },
    ],
    '@babel/preset-react',
  ],
});

CONFIG.es5 = Object.assign({}, CONFIG.default, {
  presets: [
    [
      '@babel/preset-env',
      {
        modules: 'commonjs',
      },
    ],
    '@babel/preset-react',
  ],
});

module.exports = function getConfig(api) {
  // eslint-disable-next-line
  var env = api.cache(() => process.env.BABEL_ENV || process.env.NODE_ENV);

  const config = CONFIG[env] || CONFIG.default;
  // Uncomment to debug
  console.error(env, config.plugins);
  return config;
};

module.exports.config = CONFIG.es6;

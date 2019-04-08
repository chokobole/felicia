#!/usr/bin/env bash

set -e

rm -rf dist && mkdir dist
webpack -p --env.production --config webpack.config.backend.js
# env should be changed to production once we release felicia-viz
# At this moment, becuase @felicia-viz/ packages can't be resolved.
webpack -p --env.development --env.build --config webpack.config.frontend.js
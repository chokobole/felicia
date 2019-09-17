#!/usr/bin/env bash

set -e

rm -rf dist && mkdir dist
webpack -p --env.production --config webpack.config.backend.js
webpack -p --env.production --config webpack.config.frontend.js
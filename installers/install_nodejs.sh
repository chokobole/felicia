#!/usr/bin/env bash

curl -o- https://raw.githubusercontent.com/creationix/nvm/v0.34.0/install.sh | bash

NODE_VERSION=v10.15.3

source $HOME/.nvm/nvm.sh
nvm install ${NODE_VERSION}
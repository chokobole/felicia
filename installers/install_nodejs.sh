#!/usr/bin/env bash

if ! [ -x "$(command -v nvm)" ]; then
  curl -o- https://raw.githubusercontent.com/creationix/nvm/v0.34.0/install.sh | bash

  source $HOME/.nvm/nvm.sh
fi

NODE_VERSION=v10.15.3

nvm install ${NODE_VERSION}

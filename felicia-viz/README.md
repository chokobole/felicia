# Felicia-viz

## Overview

Visualization toolkit for felicia

## How to build

```bash
npm install
npm run bootstrap
npm run build
```

And then you should prepare [felicia_js.node](/docs/installation.md#node). You should copy the output to felicia root like below.

```bash
cp bazel-bin/felicia/js/felicia_js.node .
```

## How to run

Before run node server, you need to run [master_server_main](/docs/master_server_main.md). and then run node server like below.

```bash
cd backend
npm run start:prod
```

## Contents
1. [How to use](docs/how_to_use.md)
2. [Contribution](docs/contribution.md)

# Felicia
[![Build Status](https://travis-ci.com/chokobole/felicia.svg?token=uWEvhLXsK9nuPxhDRPic&branch=master)](https://travis-ci.com/chokobole/felicia)

**Cross platform**, **Secure**, **Productive** robot framework.

## How to build
```bash
python3 scripts/felicia.py --build_all
```

## How to test
```bash
python3 scripts/felicia.py --test_all
```

## How to set up on Docker
```bash
docker build . -t felicia -f docker/Dockerfile.ubuntu
```

You can validate your work on docker with below.
```bash
python3 scripts/felicia.py --build_all --test_all --with_docker
```

### For Users
Follow this link to check [examples](felicia/examples)


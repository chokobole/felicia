#!/usr/bin/env bash

set -e

OPTION=
DEFAULT=1
DEV=0
ROS_SERIALIZATION=0

function run_cmd() {
  CMD=$@
  $CMD $OPTION
}

function install_pip() {
  if ! [ -x "$(command -v pip)" ]; then
    curl -O https://bootstrap.pypa.io/get-pip.py

  if [ -z "$PYTHON_BIN_PATH" ]; then
    PYTHON_BIN="python"
  else
    PYTHON_BIN="$PYTHON_BIN_PATH"
  fi

  run_cmd "$PYTHON_BIN get-pip.py"
  fi
}

function install_python_dep() {
  run_cmd "pip install $@"
}

function install_default_dep() {
  install_python_dep numpy
  install_python_dep grpcio
}

function install_dev_deps() {
  install_python_dep docker
}

function install_ros_serialization_dep() {
  install_python_dep empy # For ROS C++ message generation
  install_python_dep pyyaml # For ROS Python message generation
}

function run() {
  while [[ $# -gt 0 ]]
  do
    case $1 in
      --user)
        OPTION="--user"
        shift
        ;;
      --no-default)
        DEFAULT=0
        shift
        ;;
      --dev)
        DEV=1
        shift
        ;;
      --ros_serialization)
        ROS_SERIALIZATION=1
        shift
        ;;
      *)
        echo "Unknown option: $1"
        shift
        ;;
    esac
  done

  install_pip
  if [ $DEFAULT -eq 1 ]; then
    install_default_dep
  fi
  if [ $DEV -eq 1 ]; then
    install_dev_deps
  fi
  if [ $ROS_SERIALIZATION -eq 1 ]; then
    install_ros_serialization_dep
  fi
}

run "$@"

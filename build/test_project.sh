#!/bin/sh

VERSION=$(cat ../version.conf)
SCRIPT_DIR=$(dirname "$(realpath "$0")")
VORTEX_PATH="${SCRIPT_DIR}/dist/${VERSION}/bin/"
PROJECT_PATH="${SCRIPT_DIR}/../tests/project/"

bash "$VORTEX_PATH/vx.sh" \
  "$PROJECT_PATH"
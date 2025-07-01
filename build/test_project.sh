#!/bin/sh

VERSION=$(cat ../version.conf)

SCRIPT_DIR=$(dirname "$(realpath "$0")")

VORTEX_PATH="${SCRIPT_DIR}/dist/${VERSION}/bin/"
PROJECT_PATH="${SCRIPT_DIR}/../tests/project/"

SESSION_ID="editor-$(date +%m-%d-%Y-%H-%M-%S)-$(shuf -i 1000-9999 -n 1)"

cd "$PROJECT_PATH" && bash "$VORTEX_PATH/handle_crash.sh" "~/.vx/sessions/${SESSION_ID}/crash/core_dumped.txt" "$VORTEX_PATH/vortex" --editor --session_id="${SESSION_ID}"

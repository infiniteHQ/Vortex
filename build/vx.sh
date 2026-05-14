#!/bin/sh
VERSION=$(cat ../version.conf)
SCRIPT_DIR=$(dirname "$(realpath "$0")")
VORTEX_PATH="${SCRIPT_DIR}"

if [ -z "$1" ]; then
    echo "[ERROR] You need to specify your project path !"
    echo "Usage: $0 \"/path/to/project\""
    exit 1
fi

PROJECT_PATH="$1"

if [ ! -d "$PROJECT_PATH" ]; then
    echo "[ERROR] The path does not exist : \"$PROJECT_PATH\""
    exit 1
fi

SESSION_ID="editor-$(date +%m-%d-%Y-%H-%M-%S)-$(shuf -i 1000-9999 -n 1)"
cd "$PROJECT_PATH" && bash "$VORTEX_PATH/handle_crash.sh" \
"$HOME/.vx/sessions/${SESSION_ID}/crash/core_dumped.txt" \
"$VORTEX_PATH/vortex" --editor --session_id="\"$SESSION_ID\"" \
::END:: \
"$VORTEX_PATH/vortex" --crash --session_id="\"$SESSION_ID\""
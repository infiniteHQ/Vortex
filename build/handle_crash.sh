#!/bin/bash
if [ $# -lt 2 ]; then
  echo "Usage: $0 <path_to_output_file> <command_to_execute> [::END:: <end_command>]"
  exit 1
fi

OUTPUT_FILE="$1"
shift

OUTPUT_DIR=$(dirname "$OUTPUT_FILE")
mkdir -p "$OUTPUT_DIR"

# Build main command until "::END::"
COMMAND=()
while [[ $# -gt 0 ]]; do
  if [[ "$1" == "::END::" ]]; then
    shift
    break
  fi
  COMMAND+=("$1")
  shift
done

# Build end command if provided
END_COMMAND=()
while [[ $# -gt 0 ]]; do
  END_COMMAND+=("$1")
  shift
done

# Resolve vortex_utils path (same directory as this script)
SCRIPT_DIR=$(dirname "$(realpath "$0")")
VORTEX_UTILS="$SCRIPT_DIR/vortex_utils"
MANIFEST="$SCRIPT_DIR/manifest.json"

# Extract session_id from the main command arguments (--session_id="...")
SESSION_ID=""
for arg in "${COMMAND[@]}"; do
  if [[ "$arg" == --session_id=* ]]; then
    SESSION_ID="${arg#--session_id=}"
    SESSION_ID="${SESSION_ID//\"/}"
    break
  fi
done

# Extract project_path from the main command arguments (--project_path="...")
PROJECT_PATH=""
for arg in "${COMMAND[@]}"; do
  if [[ "$arg" == --project_path=* ]]; then
    PROJECT_PATH="${arg#--project_path=}"
    PROJECT_PATH="${PROJECT_PATH//\"/}"
    break
  fi
done

# Read version from manifest.json
VERSION=""
if [ -f "$MANIFEST" ]; then
  VERSION=$(grep -o '"version"\s*:\s*"[^"]*"' "$MANIFEST" | grep -o '[^"]*"$' | tr -d '"')
fi

# Get current user
USER=$(whoami)

# Register session in active_sessions.json before launching
if [ -n "$SESSION_ID" ] && [ -x "$VORTEX_UTILS" ]; then
  "$VORTEX_UTILS" -astj \
    --session_id="$SESSION_ID" \
    --version="$VERSION" \
    --user="$USER" \
    --project_path="$PROJECT_PATH"
fi

# Configure core dump pattern
echo "kernel.core_pattern = /tmp/core.%e.%p" > /etc/sysctl.d/99-core-pattern.conf
sysctl --system

# Execute the main command
echo "Executing: ${COMMAND[*]}"
"${COMMAND[@]}"
EXIT_CODE=$?

if [ $EXIT_CODE -ne 0 ]; then
  CORE_FILE=$(ls /tmp/core.* 2>/dev/null | head -n 1)
  if [ -f "$CORE_FILE" ]; then
    echo "Fatal error: Core dump found: $CORE_FILE"
    {
      echo "==== COREDUMP BEGIN ===="
      gdb -batch -ex "bt" -ex "info registers" -ex "list" "${COMMAND[0]}" "$CORE_FILE"
      echo "==== COREDUMP END ===="
    } >> "$OUTPUT_FILE" 2>&1
    rm -f "$CORE_FILE"
    echo "Core dump $CORE_FILE has been removed."
  else
    echo "Fatal error: No core dump found."
  fi

  if [ -n "$SESSION_ID" ] && [ -x "$VORTEX_UTILS" ]; then
    "$VORTEX_UTILS" -wses --session_id="$SESSION_ID" --state="failed"
  fi

  echo "Executing end command: ${END_COMMAND[*]}"
  "${END_COMMAND[@]}"
else
  echo "Command executed successfully."

  if [ -n "$SESSION_ID" ] && [ -x "$VORTEX_UTILS" ]; then
    "$VORTEX_UTILS" -wses --session_id="$SESSION_ID" --state="finished"
  fi
fi

exit $EXIT_CODE
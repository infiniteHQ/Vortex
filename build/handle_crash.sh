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

    echo "Executing end command: ${END_COMMAND[*]}"
    "${END_COMMAND[@]}"
else
    echo "Command executed successfully."
fi

exit $EXIT_CODE

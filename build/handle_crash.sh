#!/bin/bash

if [ $# -lt 2 ]; then
    echo "Usage: $0 <path_to_output_file> <command_to_execute> [<end_command>]"
    exit 1
fi

OUTPUT_FILE="$1"
shift 1

COMMAND=()
while [[ $# -gt 1 ]]; do
    COMMAND+=("\"$1\"")
    shift
done

END_COMMAND="$1"

# Configure core dump pattern
echo "kernel.core_pattern = /tmp/core.%e.%p" > /etc/sysctl.d/99-core-pattern.conf
sysctl --system

# Execute the main command
echo "Executing: ${COMMAND[*]}"
eval "${COMMAND[@]}"
EXIT_CODE=$?

if [ $EXIT_CODE -ne 0 ]; then
    CORE_FILE=$(ls /tmp/core.* 2>/dev/null | head -n 1)

    if [ -f "$CORE_FILE" ]; then
        echo "Fatal error: Core dump found: $CORE_FILE"
        echo "==== COREDUMP BEGIN ====" >> "$OUTPUT_FILE"
        gdb -batch -ex "bt" -ex "info registers" -ex "list" "${COMMAND[0]}" "$CORE_FILE" >> "$OUTPUT_FILE" 2>&1
        echo "==== COREDUMP END ====" >> "$OUTPUT_FILE"

        rm -f "$CORE_FILE"
        echo "Core dump $CORE_FILE has been removed."
    else
        echo "Fatal error: No core dump found."
    fi
else
    echo "Command executed successfully."
fi

# Execute the end command if provided
if [ -n "$END_COMMAND" ]; then
    echo "Executing end command: $END_COMMAND"
    eval "$END_COMMAND"
fi

exit $EXIT_CODE

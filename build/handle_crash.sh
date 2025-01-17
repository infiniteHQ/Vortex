#!/bin/bash

if [ $# -lt 2 ]; then
    echo "Usage: $0 <path_to_output_file> <command_to_execute>"
    exit 1
fi

OUTPUT_FILE="$1"
shift 1

COMMAND=()
for arg in "$@"; do
    COMMAND+=("\"$arg\"")
done

echo "kernel.core_pattern = /tmp/core.%e.%p" > /etc/sysctl.d/99-core-pattern.conf
sysctl --system

eval "${COMMAND[@]}"
EXIT_CODE=$?

if [ $EXIT_CODE -ne 0 ]; then
    CORE_FILE=$(ls /tmp/core.vortex.* 2>/dev/null | head -n 1)

    if [ -f "$CORE_FILE" ]; then
        echo "Vortex Fatal error handling found a core dump: $CORE_FILE"
        echo "==== COREDUMP BEGIN ===="
        gdb -batch -ex "bt" -ex "info registers" -ex "list" "$1" "$CORE_FILE" > "$OUTPUT_FILE"
        echo "==== COREDUMP END ===="

        rm -f "$CORE_FILE"
        echo "Core dump $CORE_FILE has been removed."
    else
        echo "Vortex Fatal error handling found nothing :("
    fi
fi

exit $EXIT_CODE

#!/bin/bash
set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$SCRIPT_DIR"

echo "Compiling M_ClearBox test..."
gcc -DNORMALUNIX -DLINUX -I"$SCRIPT_DIR" test_m_bbox.c m_bbox.c -o test_m_bbox -lm

echo "Running M_ClearBox test..."
./test_m_bbox

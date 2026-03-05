#!/bin/bash
set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
DOOM_DIR="${SCRIPT_DIR}/linuxdoom-1.10"

echo "Compiling m_random tests..."
gcc -DNORMALUNIX -DLINUX -I"${DOOM_DIR}" \
    "${DOOM_DIR}/test_m_random.c" "${DOOM_DIR}/m_random.c" \
    -o test_m_random

echo "Running m_random tests..."
./test_m_random

echo "Cleaning up..."
rm test_m_random

echo "All tests completed."

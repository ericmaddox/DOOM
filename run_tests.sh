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

echo "Cleaning up m_random tests..."
rm test_m_random

echo "Compiling m_misc tests..."
gcc -DNORMALUNIX -DLINUX -I"${DOOM_DIR}" \
    "${DOOM_DIR}/test_m_misc.c" \
    -o test_m_misc

echo "Running m_misc tests..."
./test_m_misc

echo "Cleaning up m_misc tests..."
rm test_m_misc

echo "All tests completed."

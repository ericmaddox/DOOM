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

echo "Compiling z_zone tests..."
gcc -DNORMALUNIX -DLINUX -I"${DOOM_DIR}" \
    "${DOOM_DIR}/test_z_zone.c" "${DOOM_DIR}/z_zone.c" \
    -o test_z_zone

echo "Running z_zone tests..."
./test_z_zone

echo "Cleaning up..."
rm test_m_random
rm test_z_zone

echo "All tests completed."

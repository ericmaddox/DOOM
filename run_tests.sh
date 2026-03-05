#!/bin/bash
set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$SCRIPT_DIR"

echo "Compiling tests..."
gcc -I./linuxdoom-1.10 tests/test_m_random.c linuxdoom-1.10/m_random.c -o tests/test_m_random -DNORMALUNIX -DLINUX

echo "Running tests..."
./tests/test_m_random

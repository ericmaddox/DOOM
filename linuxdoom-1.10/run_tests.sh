#!/bin/bash
set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$SCRIPT_DIR"

echo "Compiling tests..."
gcc -o test_m_argv test_m_argv.c m_argv.c -I.

echo "Running tests..."
./test_m_argv

echo "All tests completed successfully."

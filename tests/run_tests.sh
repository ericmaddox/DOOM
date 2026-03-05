#!/bin/bash
set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_ROOT="$SCRIPT_DIR/.."
cd "$PROJECT_ROOT"

echo "Compiling z_zone.c..."
gcc -DNORMALUNIX -DLINUX -c linuxdoom-1.10/z_zone.c -o tests/z_zone.o

echo "Compiling test_z_zone.c..."
gcc -DNORMALUNIX -DLINUX -c tests/test_z_zone.c -o tests/test_z_zone.o

echo "Linking..."
gcc tests/test_z_zone.o tests/z_zone.o -o tests/test_z_zone_runner

echo "Running tests..."
# Disable set -e for the runner so we can capture exit code
set +e
tests/test_z_zone_runner
TEST_EXIT_CODE=$?
set -e

echo "Cleaning up..."
rm tests/z_zone.o tests/test_z_zone.o tests/test_z_zone_runner

if [ $TEST_EXIT_CODE -ne 0 ]; then
    echo "Some tests failed! (Exit code: $TEST_EXIT_CODE)"
    # Force the script to fail with the correct code
    bash -c "exit $TEST_EXIT_CODE"
else
    echo "All tests passed!"
fi

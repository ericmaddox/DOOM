#!/bin/bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" > /dev/null 2>&1 && pwd )
cd "$SCRIPT_DIR"

FAILED=0

echo "Compiling and running W_LumpLength tests..."
gcc -I. -DNORMALUNIX -DLINUX -o test_w_wad test_w_wad.c w_wad.c
if [ $? -eq 0 ]; then
    ./test_w_wad
    if [ $? -ne 0 ]; then
        echo "Tests failed!"
        FAILED=1
    fi
else
    echo "Compilation failed!"
    FAILED=1
fi

echo "Compiling and running M_Random tests..."
gcc -I. -DNORMALUNIX -DLINUX -o test_m_random test_m_random.c m_random.c
if [ $? -eq 0 ]; then
    ./test_m_random
    if [ $? -ne 0 ]; then
        echo "Tests failed!"
        FAILED=1
    fi
else
    echo "Compilation failed!"
    FAILED=1
fi

echo "Running m_bbox tests..."
./run_m_bbox_test.sh
if [ $? -ne 0 ]; then
    echo "m_bbox tests failed!"
    FAILED=1
fi

echo "Running m_misc tests..."
./run_m_misc_test.sh
if [ $? -ne 0 ]; then
    echo "m_misc tests failed!"
    FAILED=1
fi

# Clean up build artifacts from run_tests.sh itself
rm -f test_w_wad test_m_random

if [ $FAILED -eq 0 ]; then
    echo "All test suites passed!"
else
    echo "Some tests failed!"
    /usr/bin/false
fi

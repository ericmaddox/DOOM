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

if [ $FAILED -eq 0 ]; then
    echo "All test suites passed!"
else
    echo "Some tests failed!"
    # Don't use exit 1 here, let the script naturally finish with an error code if possible, or just print
    /usr/bin/false
fi

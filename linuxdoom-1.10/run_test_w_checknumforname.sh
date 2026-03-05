#!/bin/bash
set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$SCRIPT_DIR"

mkdir -p linux

# Compile w_wad.c
gcc -DNORMALUNIX -DLINUX -c w_wad.c -o linux/w_wad.o

# Compile the test
gcc -DNORMALUNIX -DLINUX -I. -c test_w_checknumforname.c -o linux/test_w_checknumforname.o

# Link them together
gcc -o test_w_checknumforname linux/test_w_checknumforname.o linux/w_wad.o

# Run the tests
./test_w_checknumforname

# Clean up
rm test_w_checknumforname linux/test_w_checknumforname.o

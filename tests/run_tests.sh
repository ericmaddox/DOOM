#!/bin/bash
set -e
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$SCRIPT_DIR/.."
mkdir -p build
gcc -DNORMALUNIX -DLINUX -I./linuxdoom-1.10 -o build/test_z_zone tests/test_z_zone.c linuxdoom-1.10/z_zone.c
build/test_z_zone

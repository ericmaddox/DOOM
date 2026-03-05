#!/bin/bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" > /dev/null 2>&1 && pwd )
cd "$SCRIPT_DIR"

FAILED=0

echo "Compiling and running M_WriteFile tests..."

# Extract M_WriteFile code for testing. We need to extract from m_misc.c so tests actually cover the real file.
cat << 'C_EOF' > m_misc_writefile.c
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "doomdef.h"
#include "doomtype.h"
C_EOF

awk '/#ifndef O_BINARY/{flag=1} flag {print; if (/^}/) {flag=0; exit}}' m_misc.c >> m_misc_writefile.c

gcc -I. -DNORMALUNIX -DLINUX -c m_misc_writefile.c
if [ $? -ne 0 ]; then
    echo "Compilation of M_WriteFile stub failed!"
    FAILED=1
fi

if [ $FAILED -eq 0 ]; then
    gcc -I. -DNORMALUNIX -DLINUX -o test_m_misc test_m_misc.c m_misc_writefile.o
    if [ $? -eq 0 ]; then
        ./test_m_misc
        if [ $? -ne 0 ]; then
            echo "Tests failed!"
            FAILED=1
        fi
    else
        echo "Compilation of test failed!"
        FAILED=1
    fi
fi

# Clean up
rm -f test_m_misc test_m_misc.o m_misc_writefile.c m_misc_writefile.o no_perm_test.tmp test_write.tmp

if [ $FAILED -eq 0 ]; then
    echo "M_WriteFile tests passed!"
else
    echo "M_WriteFile tests failed!"
    /usr/bin/false
fi

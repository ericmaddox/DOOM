#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <setjmp.h>
#include <assert.h>

#include "z_zone.h"
#include "i_system.h"
#include "doomdef.h"

jmp_buf error_jmp;
char last_error[1024];

void I_Error(char *error, ...) {
    va_list argptr;
    va_start(argptr, error);
    vsnprintf(last_error, sizeof(last_error), error, argptr);
    va_end(argptr);
    longjmp(error_jmp, 1);
}

byte* I_ZoneBase(int *size) {
    *size = 1024 * 1024;
    return malloc(*size);
}

void test_Z_Free_error_path() {
    void* ptr;
    memblock_t* block;

    printf("Testing Z_Free error path...\n");

    // Initialize zone
    Z_Init();

    // Allocate a block
    ptr = Z_Malloc(64, PU_STATIC, NULL);
    assert(ptr != NULL);

    // Get the block header
    block = (memblock_t *)((byte *)ptr - sizeof(memblock_t));

    // Corrupt the ID
    block->id = 0xDEADBEEF; // Not ZONEID

    if (setjmp(error_jmp) == 0) {
        Z_Free(ptr);
        assert(0 && "Z_Free should have called I_Error");
    } else {
        // Returned from longjmp
        assert(strstr(last_error, "Z_Free: freed a pointer without ZONEID") != NULL);
        printf("Z_Free error path passed! Error caught: %s\n", last_error);
    }
}

int main() {
    test_Z_Free_error_path();
    printf("All z_zone tests passed successfully!\n");
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <string.h>
#include "z_zone.h"
#include "doomtype.h"

jmp_buf error_jmp;
int error_called = 0;
char error_msg[256];

void I_Error(char *error, ...) {
    error_called = 1;
    strncpy(error_msg, error, sizeof(error_msg) - 1);
    error_msg[sizeof(error_msg) - 1] = '\0';
    longjmp(error_jmp, 1);
}

// Dummy I_ZoneBase for Z_Init
byte *I_ZoneBase(int *size) {
    *size = 1024 * 1024; // 1 MB
    return (byte *)malloc(*size);
}

void test_z_free_missing_zoneid() {
    void *ptr;
    memblock_t *block;

    printf("Running test_z_free_missing_zoneid...\n");

    // Allocate a block manually to bypass Z_Malloc which sets ZONEID
    block = (memblock_t *)malloc(sizeof(memblock_t) + 64);
    block->id = 0; // Deliberately bad ID

    ptr = (void *)((byte *)block + sizeof(memblock_t));

    error_called = 0;
    error_msg[0] = '\0';

    if (setjmp(error_jmp) == 0) {
        Z_Free(ptr);
        printf("FAIL: I_Error was not called for Z_Free without ZONEID\n");
        exit(1);
    } else {
        if (error_called) {
            if (strstr(error_msg, "Z_Free: freed a pointer without ZONEID") != NULL) {
                printf("PASS: I_Error called for Z_Free without ZONEID with correct message\n");
            } else {
                printf("FAIL: I_Error called with wrong message: %s\n", error_msg);
                exit(1);
            }
        } else {
            printf("FAIL: Error jump occurred but error_called flag not set\n");
            exit(1);
        }
    }

    free(block);
}

int main() {
    printf("Running Z_Free tests...\n");
    Z_Init(); // Initialize zone memory, uses I_ZoneBase mock
    test_z_free_missing_zoneid();
    printf("All Z_Free tests passed.\n");
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <string.h>
#include "w_wad.h"
#include "z_zone.h"

jmp_buf error_jmp;
int error_called = 0;
char error_msg[256];

void I_Error(char *error, ...) {
    error_called = 1;
    strncpy(error_msg, error, sizeof(error_msg) - 1);
    error_msg[sizeof(error_msg) - 1] = '\0';
    longjmp(error_jmp, 1);
}

void Z_Free(void *ptr) {
    free(ptr);
}

void *Z_Malloc(int size, int tag, void *user) {
    return malloc(size);
}

void Z_ChangeTag2(void *ptr, int tag) {
    // Mock implementation for testing
}

void test_valid_lump() {
    lumpinfo_t test_lumps[3] = {
        {"LUMP0", 0, 0, 100},
        {"LUMP1", 0, 0, 250},
        {"LUMP2", 0, 0, 500}
    };
    lumpinfo = test_lumps;
    numlumps = 3;

    if (W_LumpLength(0) != 100) {
        printf("FAIL: Expected 100 for lump 0\n");
        exit(1);
    }
    if (W_LumpLength(1) != 250) {
        printf("FAIL: Expected 250 for lump 1\n");
        exit(1);
    }
    if (W_LumpLength(2) != 500) {
        printf("FAIL: Expected 500 for lump 2\n");
        exit(1);
    }

    printf("PASS: Valid lump length retrieved correctly\n");
}

void test_out_of_bounds_lump() {
    lumpinfo_t test_lumps[1] = {
        {"LUMP0", 0, 0, 100}
    };
    lumpinfo = test_lumps;
    numlumps = 1;
    error_called = 0;
    error_msg[0] = '\0';

    if (setjmp(error_jmp) == 0) {
        W_LumpLength(1);
        printf("FAIL: I_Error was not called for out-of-bounds lump\n");
        exit(1);
    } else {
        if (error_called) {
            if (strstr(error_msg, "W_LumpLength: %i >= numlumps") != NULL) {
                printf("PASS: I_Error called for out-of-bounds lump with correct message\n");
            } else {
                printf("FAIL: I_Error called with wrong message: %s\n", error_msg);
                exit(1);
            }
        } else {
            printf("FAIL: Error jump occurred but error_called flag not set\n");
            exit(1);
        }
    }
}

int main() {
    printf("Running W_LumpLength tests...\n");
    test_valid_lump();
    test_out_of_bounds_lump();
    printf("All W_LumpLength tests passed.\n");
    return 0;
}

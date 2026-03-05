#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "doomtype.h"
#include "w_wad.h"

// Provide necessary definitions that are missing from other object files
void I_Error(char *error, ...) {
    printf("I_Error called: %s\n", error);
    exit(1);
}

void Z_Free(void *ptr) {}
void* Z_Malloc(int size, int tag, void *user) { return malloc(size); }
void Z_ChangeTag2(void *ptr, int tag) {}

int main() {
    int failed = 0;

    // Set up mock lumpinfo
    numlumps = 4;
    lumpinfo = (lumpinfo_t*)malloc(numlumps * sizeof(lumpinfo_t));

    // Lump 0
    strncpy(lumpinfo[0].name, "LUMP1\0\0\0", 8); // need exact layout
    // Lump 1
    strncpy(lumpinfo[1].name, "LUMP2\0\0\0", 8);
    // Lump 2 - A patch lump with the same name, should take precedence
    strncpy(lumpinfo[2].name, "LUMP1\0\0\0", 8);
    // Lump 3 - An exact 8-character lump name, no null terminator
    strncpy(lumpinfo[3].name, "LUMPEXCT", 8);

    printf("Running W_CheckNumForName tests...\n");

    // Test 1: Basic find
    printf("Test 1: Find LUMP2\n");
    if (W_CheckNumForName("LUMP2") != 1) {
        printf("  Failed: expected 1, got %d\n", W_CheckNumForName("LUMP2"));
        failed++;
    }

    // Test 2: Precedence (scan backwards)
    printf("Test 2: Precedence of LUMP1\n");
    if (W_CheckNumForName("LUMP1") != 2) {
        printf("  Failed: expected 2, got %d\n", W_CheckNumForName("LUMP1"));
        failed++;
    }

    // Test 3: Not found
    printf("Test 3: Not found\n");
    if (W_CheckNumForName("LUMP3") != -1) {
        printf("  Failed: expected -1, got %d\n", W_CheckNumForName("LUMP3"));
        failed++;
    }

    // Test 4: Case insensitivity
    printf("Test 4: Case insensitivity\n");
    if (W_CheckNumForName("lump2") != 1) {
        printf("  Failed: expected 1, got %d\n", W_CheckNumForName("lump2"));
        failed++;
    }

    // Test 5: Exact 8-character name
    printf("Test 5: Exact 8 char name\n");
    if (W_CheckNumForName("LUMPEXCT") != 3) {
        printf("  Failed: expected 3, got %d\n", W_CheckNumForName("LUMPEXCT"));
        failed++;
    }

    // Edge Case Tests

    // Test 6: Input > 8 characters
    printf("Test 6: Input > 8 characters\n");
    if (W_CheckNumForName("LUMPEXCTEXTRA") != 3) {
        printf("  Failed: expected 3, got %d\n", W_CheckNumForName("LUMPEXCTEXTRA"));
        failed++;
    }

    // Test 7: Empty string
    printf("Test 7: Empty string\n");
    if (W_CheckNumForName("") != -1) {
        printf("  Failed: expected -1, got %d\n", W_CheckNumForName(""));
        failed++;
    }

    // Test 8: Empty lumpinfo (numlumps = 0)
    printf("Test 8: numlumps = 0\n");
    int old_numlumps = numlumps;
    numlumps = 0;
    if (W_CheckNumForName("LUMP1") != -1) {
        printf("  Failed: expected -1, got %d\n", W_CheckNumForName("LUMP1"));
        failed++;
    }
    numlumps = old_numlumps; // Restore

    // Clean up
    free(lumpinfo);

    if (failed) {
        printf("TESTS FAILED: %d\n", failed);
        return 1;
    } else {
        printf("ALL TESTS PASSED\n");
        return 0;
    }
}

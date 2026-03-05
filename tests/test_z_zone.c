#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>

#include "../linuxdoom-1.10/doomdef.h"
#include "../linuxdoom-1.10/z_zone.h"
#include "../linuxdoom-1.10/i_system.h"

jmp_buf err_buf;
char last_error[256];

void I_Error(char *error, ...) {
    strncpy(last_error, error, sizeof(last_error) - 1);
    last_error[sizeof(last_error) - 1] = 0;
    longjmp(err_buf, 1);
}

byte* I_ZoneBase (int* size) {
    *size = 1024;
    return (byte*)malloc(1024);
}

typedef struct
{
    int		size;
    memblock_t	blocklist;
    memblock_t*	rover;
} memzone_t;

extern memzone_t* mainzone;

int test_heap() {
    int ret;
    if ((ret = setjmp(err_buf)) == 0) {
        Z_CheckHeap();
        return 0; // Success
    }
    return 1; // Failed (I_Error called)
}

int main() {
    printf("Starting Z_CheckHeap tests\n");
    int failures = 0;
    int test_result;

    int zone_size = sizeof(memzone_t) + 3 * sizeof(memblock_t) + 100;
    mainzone = (memzone_t*)malloc(zone_size);
    mainzone->size = zone_size;

    mainzone->blocklist.user = (void*)mainzone;
    mainzone->blocklist.tag = PU_STATIC;

    memblock_t* b1 = (memblock_t*)((byte*)mainzone + sizeof(memzone_t));
    memblock_t* b2 = (memblock_t*)((byte*)b1 + sizeof(memblock_t) + 20);
    memblock_t* b3 = (memblock_t*)((byte*)b2 + sizeof(memblock_t) + 30);

    mainzone->blocklist.next = b1;
    mainzone->blocklist.prev = b3;

    b1->prev = &mainzone->blocklist;
    b1->next = b2;
    b1->size = sizeof(memblock_t) + 20;
    b1->user = (void**)1;

    b2->prev = b1;
    b2->next = b3;
    b2->size = sizeof(memblock_t) + 30;
    b2->user = (void**)1;

    b3->prev = b2;
    b3->next = &mainzone->blocklist;
    b3->size = zone_size - sizeof(memzone_t) - b1->size - b2->size;
    b3->user = (void**)1;

    printf("Test 1: Valid heap...\n");
    test_result = test_heap();
    if (test_result == 0) {
        printf("  PASS\n");
    } else {
        printf("  FAIL (unexpected error: %s)\n", last_error);
        failures++;
    }

    printf("Test 2: Block size mismatch...\n");
    int old_size = b1->size;
    b1->size -= 4;
    test_result = test_heap();
    if (test_result == 1 && strstr(last_error, "does not touch the next block")) {
        printf("  PASS\n");
    } else {
        printf("  FAIL (expected 'does not touch the next block', got %s)\n", test_result == 0 ? "success" : last_error);
        failures++;
    }
    b1->size = old_size; // Restore

    printf("Test 3: Bad back link...\n");
    memblock_t* old_prev = b2->prev;
    b2->prev = NULL;
    test_result = test_heap();
    if (test_result == 1 && strstr(last_error, "proper back link")) {
        printf("  PASS\n");
    } else {
        printf("  FAIL (expected 'proper back link', got %s)\n", test_result == 0 ? "success" : last_error);
        failures++;
    }
    b2->prev = old_prev; // Restore

    printf("Test 4: Consecutive free blocks...\n");
    void** old_u1 = b1->user;
    void** old_u2 = b2->user;
    b1->user = NULL;
    b2->user = NULL;
    test_result = test_heap();
    if (test_result == 1 && strstr(last_error, "two consecutive free blocks")) {
        printf("  PASS\n");
    } else {
        printf("  FAIL (expected 'two consecutive free blocks', got %s)\n", test_result == 0 ? "success" : last_error);
        failures++;
    }
    b1->user = old_u1; // Restore
    b2->user = old_u2; // Restore

    printf("Test 5: Free block followed by used block...\n");
    old_u1 = b1->user;
    b1->user = NULL; // b1 is free, b2 is used
    test_result = test_heap();
    if (test_result == 0) {
        printf("  PASS\n");
    } else {
        printf("  FAIL (unexpected error: %s)\n", last_error);
        failures++;
    }
    b1->user = old_u1;

    free(mainzone);
    printf("Done. %d failures.\n", failures);

    return failures > 0 ? 1 : 0;
}

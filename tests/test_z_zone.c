#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

typedef unsigned char byte;

#include "z_zone.h"
#include "doomdef.h"

// Redefine internal structs so we can test them
typedef struct
{
    // total bytes malloced, including header
    int		size;

    // start / end cap for linked list
    memblock_t	blocklist;

    memblock_t*	rover;

} memzone_t;

// Extern the globals from z_zone.c
extern memzone_t* mainzone;

// Stub out dependencies from i_system.c
int mb_used = 6;
byte* I_ZoneBase (int* size)
{
    *size = mb_used*1024*1024;
    return (byte *) malloc (*size);
}

void I_Error(char *error, ...) {
    printf("I_Error called: %s\n", error);
    exit(1);
}

void test_z_init() {
    printf("Running test_z_init...\n");

    // Call Z_Init
    Z_Init();

    // Test 1: mainzone is allocated and initialized
    assert(mainzone != NULL);

    // Test 2: size is set correctly
    int expected_size = mb_used * 1024 * 1024;
    assert(mainzone->size == expected_size);

    // Test 3: The blocklist user is set to mainzone itself
    assert(mainzone->blocklist.user == (void*)mainzone);

    // Test 4: The blocklist tag is PU_STATIC
    assert(mainzone->blocklist.tag == PU_STATIC);

    // Test 5: The rover points to the first block
    memblock_t* first_block = (memblock_t*)( (byte *)mainzone + sizeof(memzone_t) );
    assert(mainzone->rover == first_block);

    // Test 6: The block list forms a circular doubly linked list with the first block
    assert(mainzone->blocklist.next == first_block);
    assert(mainzone->blocklist.prev == first_block);
    assert(first_block->next == &mainzone->blocklist);
    assert(first_block->prev == &mainzone->blocklist);

    // Test 7: First block user is NULL (free block)
    assert(first_block->user == NULL);

    // Test 8: First block size covers remaining space
    assert(first_block->size == expected_size - sizeof(memzone_t));

    printf("test_z_init passed!\n");
}

void test_z_init_various_sizes() {
    printf("Running test_z_init_various_sizes...\n");

    int old_mb_used = mb_used;

    // Test with minimum practical size (1 MB)
    mb_used = 1;
    Z_Init();
    assert(mainzone != NULL);
    assert(mainzone->size == 1 * 1024 * 1024);

    // Test with large size (32 MB)
    mb_used = 32;
    Z_Init();
    assert(mainzone != NULL);
    assert(mainzone->size == 32 * 1024 * 1024);

    // Restore
    mb_used = old_mb_used;
    Z_Init();

    printf("test_z_init_various_sizes passed!\n");
}

int main() {
    printf("Testing Z_Init...\n");
    test_z_init();
    test_z_init_various_sizes();
    printf("All tests passed.\n");
    return 0;
}

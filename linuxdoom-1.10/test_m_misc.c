#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <sys/stat.h>
#include "doomtype.h"

// Declaration of M_WriteFile from m_misc.h
boolean M_WriteFile(char const* name, void* source, int length);

void test_successful_write() {
    printf("Testing successful write...\n");
    char* filename = "test_write.tmp";
    char* content = "Hello Doom!";
    boolean result = M_WriteFile(filename, content, strlen(content));
    assert(result == true);

    // Check file contents
    FILE* f = fopen(filename, "r");
    assert(f != NULL);
    char buf[64] = {0};
    fread(buf, 1, strlen(content), f);
    fclose(f);
    assert(strcmp(buf, content) == 0);
    remove(filename);
    printf("Successful write passed.\n");
}

void test_write_to_invalid_path() {
    printf("Testing write to invalid path...\n");
    // Trying to write to a directory that does not exist
    char* filename = "non_existent_dir/test_write.tmp";
    char* content = "This should fail.";
    boolean result = M_WriteFile(filename, content, strlen(content));
    assert(result == false);
    printf("Write to invalid path passed.\n");
}

void test_write_no_permission() {
    printf("Testing write without permissions...\n");
    char* filename = "no_perm_test.tmp";

    // Create file and remove write permissions
    FILE* f = fopen(filename, "w");
    assert(f != NULL);
    fclose(f);
    chmod(filename, 0444); // Read-only

    char* content = "This should fail.";
    boolean result = M_WriteFile(filename, content, strlen(content));
    assert(result == false);

    remove(filename); // Clean up
    printf("Write without permissions passed.\n");
}

int main() {
    test_successful_write();
    test_write_to_invalid_path();
    test_write_no_permission();
    return 0;
}

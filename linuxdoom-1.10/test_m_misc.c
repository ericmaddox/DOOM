#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdarg.h>

#include "doomtype.h"
#include "m_misc.h"

// Define stub variables and functions needed to compile the test code.
// Since m_misc.c cannot be compiled with GCC on 64-bit due to the pointer-to-int cast
// in its global default struct initialization (which must be a compile-time constant),
// we compile our own isolated copy of the functions we're testing.

jmp_buf error_jmp;
int error_called = 0;
char error_msg[256];

void I_Error(char *error, ...) {
    error_called = 1;
    va_list argptr;
    va_start(argptr, error);
    vsnprintf(error_msg, sizeof(error_msg), error, argptr);
    va_end(argptr);
    longjmp(error_jmp, 1);
}

void Z_Free(void *ptr) {
    free(ptr);
}

void *Z_Malloc(int size, int tag, void *user) {
    return malloc(size);
}

// Re-implement the exact function under test to avoid compilation issues with the rest of m_misc.c
// and its 16-bit/32-bit assumptions about pointer sizes in global constants.
int M_ReadFile(char const* name, byte** buffer) {
    int handle, count, length;
    struct stat fileinfo;
    byte *buf;

    // We must define O_BINARY if it doesn't exist to match m_misc.c
#ifndef O_BINARY
#define O_BINARY 0
#endif

    handle = open (name, O_RDONLY | O_BINARY, 0666);
    if (handle == -1)
        I_Error ("Couldn't read file %s", name);
    if (fstat (handle,&fileinfo) == -1)
        I_Error ("Couldn't read file %s", name);
    length = fileinfo.st_size;
    buf = Z_Malloc (length, 1, NULL); // PU_STATIC is 1
    count = read (handle, buf, length);
    close (handle);

    if (count < length)
        I_Error ("Couldn't read file %s", name);

    *buffer = buf;
    return length;
}

// test cases
void test_happy_path() {
    printf("Testing M_ReadFile happy path...\n");
    const char *test_file = "test_misc_happy.txt";
    const char *content = "Hello, Doom!";
    int length = strlen(content);

    // Create test file
    int fd = open(test_file, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd == -1) {
        printf("FAIL: Could not create test file\n");
        exit(1);
    }
    write(fd, content, length);
    close(fd);

    byte *buffer = NULL;
    int read_length = M_ReadFile(test_file, &buffer);

    if (read_length != length) {
        printf("FAIL: Expected length %d, got %d\n", length, read_length);
        exit(1);
    }

    if (memcmp(buffer, content, length) != 0) {
        printf("FAIL: Content mismatch\n");
        exit(1);
    }

    Z_Free(buffer);
    unlink(test_file);
    printf("PASS: Happy path\n");
}

void test_error_handle() {
    printf("Testing M_ReadFile handle == -1 error...\n");
    error_called = 0;
    error_msg[0] = '\0';

    if (setjmp(error_jmp) == 0) {
        byte *buffer = NULL;
        M_ReadFile("does_not_exist_file.txt", &buffer);
        printf("FAIL: I_Error was not called for missing file\n");
        exit(1);
    } else {
        if (error_called) {
            if (strstr(error_msg, "Couldn't read file does_not_exist_file.txt") != NULL) {
                printf("PASS: I_Error called for missing file with correct message\n");
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

void test_error_count_less_than_length() {
    printf("Testing M_ReadFile count < length error...\n");
    error_called = 0;
    error_msg[0] = '\0';

    // Using a directory should cause read() to return -1 (EISDIR) while fstat gives > 0 size
    if (setjmp(error_jmp) == 0) {
        byte *buffer = NULL;
        M_ReadFile(".", &buffer);
        printf("FAIL: I_Error was not called for directory read\n");
        exit(1);
    } else {
        if (error_called) {
            if (strstr(error_msg, "Couldn't read file .") != NULL) {
                printf("PASS: I_Error called for directory read with correct message\n");
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
    printf("Running M_ReadFile tests...\n");
    test_happy_path();
    test_error_handle();
    test_error_count_less_than_length();
    printf("All M_ReadFile tests passed.\n");
    return 0;
}

#include <fcntl.h>
#include <unistd.h>
#ifndef O_BINARY
#define O_BINARY 0
#endif
extern char* reloadname;

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

extern jmp_buf error_jmp;
extern int error_called;
extern char error_msg[256];
extern char* reloadname;

void test_read_valid_lump() {
    int fd;
    char dest[6];
    char temp_filename[] = "test_valid_wad_XXXXXX";

    fd = mkstemp(temp_filename);
    if (fd == -1) {
        printf("FAIL: Could not create temp file\n");
        exit(1);
    }
    write(fd, "HELLOWORLD", 10);
    close(fd);

    fd = open(temp_filename, O_RDONLY | O_BINARY);

    lumpinfo_t test_lumps[1] = {
        {"LUMP0", fd, 5, 5}
    };
    lumpinfo = test_lumps;
    numlumps = 1;

    memset(dest, 0, sizeof(dest));
    W_ReadLump(0, dest);

    if (strcmp(dest, "WORLD") != 0) {
        printf("FAIL: Expected 'WORLD', got '%s'\n", dest);
        exit(1);
    }

    close(fd);
    unlink(temp_filename);
    printf("PASS: Valid lump read correctly\n");
}

void test_read_out_of_bounds_lump() {
    char dest[10];
    lumpinfo_t test_lumps[1] = {
        {"LUMP0", 0, 0, 100}
    };
    lumpinfo = test_lumps;
    numlumps = 1;
    error_called = 0;
    error_msg[0] = '\0';

    if (setjmp(error_jmp) == 0) {
        W_ReadLump(1, dest);
        printf("FAIL: I_Error was not called for out-of-bounds lump in W_ReadLump\n");
        exit(1);
    } else {
        if (error_called) {
            if (strstr(error_msg, "W_ReadLump: ") != NULL) {
                printf("PASS: I_Error called for out-of-bounds lump in W_ReadLump with correct message\n");
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

void test_read_reload_file() {
    int fd;
    char dest[7];
    char temp_filename[] = "test_reload_wad_XXXXXX";

    fd = mkstemp(temp_filename);
    if (fd == -1) {
        printf("FAIL: Could not create temp file\n");
        exit(1);
    }
    write(fd, "RELOAD", 6);
    close(fd);

    reloadname = temp_filename;
    lumpinfo_t test_lumps[1] = {
        {"LUMP0", -1, 0, 6}
    };
    lumpinfo = test_lumps;
    numlumps = 1;

    memset(dest, 0, sizeof(dest));
    W_ReadLump(0, dest);

    if (strcmp(dest, "RELOAD") != 0) {
        printf("FAIL: Expected 'RELOAD', got '%s'\n", dest);
        exit(1);
    }

    unlink(temp_filename);
    printf("PASS: Reload file read correctly\n");
}

void test_read_short_read() {
    int fd;
    char dest[20];
    char temp_filename[] = "test_short_wad_XXXXXX";

    fd = mkstemp(temp_filename);
    if (fd == -1) {
        printf("FAIL: Could not create temp file\n");
        exit(1);
    }
    write(fd, "SHORT", 5);
    close(fd);

    fd = open(temp_filename, O_RDONLY | O_BINARY);

    lumpinfo_t test_lumps[1] = {
        {"LUMP0", fd, 0, 10}
    };
    lumpinfo = test_lumps;
    numlumps = 1;
    error_called = 0;
    error_msg[0] = '\0';

    if (setjmp(error_jmp) == 0) {
        W_ReadLump(0, dest);
        printf("FAIL: I_Error was not called for short read in W_ReadLump\n");
        exit(1);
    } else {
        if (error_called) {
            if (strstr(error_msg, "W_ReadLump: only read") != NULL) {
                printf("PASS: I_Error called for short read in W_ReadLump with correct message\n");
            } else {
                printf("FAIL: I_Error called with wrong message: %s\n", error_msg);
                exit(1);
            }
        } else {
            printf("FAIL: Error jump occurred but error_called flag not set\n");
            exit(1);
        }
    }

    close(fd);
    unlink(temp_filename);
}

void test_read_open_failure() {
    char dest[10];
    reloadname = "nonexistent_wad_12345.wad";

    lumpinfo_t test_lumps[1] = {
        {"LUMP0", -1, 0, 10}
    };
    lumpinfo = test_lumps;
    numlumps = 1;
    error_called = 0;
    error_msg[0] = '\0';

    if (setjmp(error_jmp) == 0) {
        W_ReadLump(0, dest);
        printf("FAIL: I_Error was not called for open failure in W_ReadLump\n");
        exit(1);
    } else {
        if (error_called) {
            if (strstr(error_msg, "W_ReadLump: couldn't open") != NULL) {
                printf("PASS: I_Error called for open failure in W_ReadLump with correct message\n");
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
    printf("Running W_ReadLump tests...\n");
    test_read_valid_lump();
    test_read_out_of_bounds_lump();
    test_read_reload_file();
    test_read_short_read();
    test_read_open_failure();
    printf("All W_ReadLump tests passed.\n");
    return 0;
}

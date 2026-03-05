#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

extern int myargc;
extern char** myargv;

int M_CheckParm (char *check);

void test_M_CheckParm() {
    char* args[] = {
        "doom", // 0
        "-skill", // 1
        "4", // 2
        "-warp", // 3
        "1", // 4
        "1", // 5
        "-fast", // 6
        "-NOMONSTERS" // 7
    };

    myargc = sizeof(args) / sizeof(args[0]);
    myargv = args;

    // Test for program name (should return 0 because it checks from index 1)
    if (M_CheckParm("doom") != 0) {
        printf("Failed: M_CheckParm('doom') returned %d, expected 0\n", M_CheckParm("doom"));
        exit(1);
    }

    // Test for present parameter
    if (M_CheckParm("-skill") != 1) {
        printf("Failed: M_CheckParm('-skill') returned %d, expected 1\n", M_CheckParm("-skill"));
        exit(1);
    }

    // Test for another present parameter
    if (M_CheckParm("-warp") != 3) {
        printf("Failed: M_CheckParm('-warp') returned %d, expected 3\n", M_CheckParm("-warp"));
        exit(1);
    }

    // Test for case insensitivity
    if (M_CheckParm("-nomonsters") != 7) {
        printf("Failed: M_CheckParm('-nomonsters') returned %d, expected 7\n", M_CheckParm("-nomonsters"));
        exit(1);
    }
    if (M_CheckParm("-NOMONSTERS") != 7) {
        printf("Failed: M_CheckParm('-NOMONSTERS') returned %d, expected 7\n", M_CheckParm("-NOMONSTERS"));
        exit(1);
    }

    // Test for missing parameter
    if (M_CheckParm("-missing") != 0) {
        printf("Failed: M_CheckParm('-missing') returned %d, expected 0\n", M_CheckParm("-missing"));
        exit(1);
    }

    // Test for empty args list (only program name)
    myargc = 1;
    if (M_CheckParm("-skill") != 0) {
        printf("Failed: Empty args list returned %d, expected 0\n", M_CheckParm("-skill"));
        exit(1);
    }

    // Test for zero args list
    myargc = 0;
    if (M_CheckParm("-skill") != 0) {
        printf("Failed: Zero args list returned %d, expected 0\n", M_CheckParm("-skill"));
        exit(1);
    }

    printf("All M_CheckParm tests passed!\n");
}

int main() {
    test_M_CheckParm();
    return 0;
}

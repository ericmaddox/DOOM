#include <stdio.h>
#include <stdlib.h>
#include "m_random.h"

int main() {
    int i;
    int seq1[10];
    int seq2[10];
    int pseq1[10];
    int pseq2[10];

    // Clear random at start
    M_ClearRandom();

    // Generate first sequence for M_Random
    for (i = 0; i < 10; i++) {
        seq1[i] = M_Random();
    }

    // Generate first sequence for P_Random
    for (i = 0; i < 10; i++) {
        pseq1[i] = P_Random();
    }

    // Advance state arbitrarily
    for (i = 0; i < 17; i++) {
        M_Random();
        P_Random();
    }

    // Clear random again
    M_ClearRandom();

    // Generate second sequence for M_Random
    for (i = 0; i < 10; i++) {
        seq2[i] = M_Random();
    }

    // Generate second sequence for P_Random
    for (i = 0; i < 10; i++) {
        pseq2[i] = P_Random();
    }

    // Verify sequences match exactly
    for (i = 0; i < 10; i++) {
        if (seq1[i] != seq2[i]) {
            printf("FAIL: M_Random sequence mismatch at index %d (expected %d, got %d)\n", i, seq1[i], seq2[i]);
            return 1;
        }
    }

    for (i = 0; i < 10; i++) {
        if (pseq1[i] != pseq2[i]) {
            printf("FAIL: P_Random sequence mismatch at index %d (expected %d, got %d)\n", i, pseq1[i], pseq2[i]);
            return 1;
        }
    }

    printf("PASS: M_ClearRandom successfully resets deterministic random state.\n");
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "m_random.h"

// We can define rndtable here to test against the exact values,
// or just test the properties (e.g., wraparound, clearing).
extern unsigned char rndtable[256];

void test_M_ClearRandom() {
    printf("Testing M_ClearRandom...\n");
    // Call it a few times
    M_Random();
    M_Random();
    P_Random();

    // Clear it
    M_ClearRandom();

    // The first call after clear should return rndtable[1]
    // because rndindex is 0, then (0+1)&0xff is 1.
    // Wait, let's look at M_Random implementation:
    // int M_Random (void) {
    //     rndindex = (rndindex+1)&0xff;
    //     return rndtable[rndindex];
    // }
    // Yes, after M_ClearRandom(), rndindex is 0, so next call returns rndtable[1].
    int val1 = M_Random();
    int p_val1 = P_Random();

    assert(val1 == rndtable[1]);
    assert(p_val1 == rndtable[1]);
    printf("M_ClearRandom passed.\n");
}

void test_M_Random_wraparound() {
    printf("Testing M_Random wraparound...\n");
    M_ClearRandom();

    // Loop 255 times, so rndindex becomes 255.
    for (int i = 0; i < 255; i++) {
        M_Random();
    }

    // Now rndindex is 255.
    // Next call should wrap around to 0.
    int wrap_val = M_Random();
    assert(wrap_val == rndtable[0]);

    // Next call should be 1.
    int next_val = M_Random();
    assert(next_val == rndtable[1]);
    printf("M_Random wraparound passed.\n");
}

void test_P_Random_wraparound() {
    printf("Testing P_Random wraparound...\n");
    M_ClearRandom();

    // Loop 255 times, so prndindex becomes 255.
    for (int i = 0; i < 255; i++) {
        P_Random();
    }

    // Now prndindex is 255.
    // Next call should wrap around to 0.
    int wrap_val = P_Random();
    assert(wrap_val == rndtable[0]);

    // Next call should be 1.
    int next_val = P_Random();
    assert(next_val == rndtable[1]);
    printf("P_Random wraparound passed.\n");
}

void test_independent_indices() {
    printf("Testing independent indices for M_Random and P_Random...\n");
    M_ClearRandom();

    // Advance M_Random
    M_Random();
    M_Random();

    // P_Random should still be at the beginning
    int p_val = P_Random();
    assert(p_val == rndtable[1]);

    int m_val = M_Random();
    assert(m_val == rndtable[3]);
    printf("Independent indices passed.\n");
}

int main() {
    printf("Running m_random tests...\n");

    test_M_ClearRandom();
    test_M_Random_wraparound();
    test_P_Random_wraparound();
    test_independent_indices();

    printf("All m_random tests passed successfully!\n");
    return 0;
}

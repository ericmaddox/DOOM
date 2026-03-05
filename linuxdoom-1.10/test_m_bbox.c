#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "doomdef.h"
#include "m_bbox.h"

int main() {
    fixed_t box[4];

    // Initialize with dummy values
    box[BOXTOP] = 0;
    box[BOXBOTTOM] = 0;
    box[BOXLEFT] = 0;
    box[BOXRIGHT] = 0;

    printf("Running M_ClearBox test...\n");
    M_ClearBox(box);

    int passed = 1;

    if (box[BOXTOP] != MININT) {
        printf("FAIL: box[BOXTOP] != MININT (Expected: %d, Got: %d)\n", MININT, box[BOXTOP]);
        passed = 0;
    }
    if (box[BOXRIGHT] != MININT) {
        printf("FAIL: box[BOXRIGHT] != MININT (Expected: %d, Got: %d)\n", MININT, box[BOXRIGHT]);
        passed = 0;
    }
    if (box[BOXBOTTOM] != MAXINT) {
        printf("FAIL: box[BOXBOTTOM] != MAXINT (Expected: %d, Got: %d)\n", MAXINT, box[BOXBOTTOM]);
        passed = 0;
    }
    if (box[BOXLEFT] != MAXINT) {
        printf("FAIL: box[BOXLEFT] != MAXINT (Expected: %d, Got: %d)\n", MAXINT, box[BOXLEFT]);
        passed = 0;
    }

    if (passed) {
        printf("SUCCESS: M_ClearBox initialized bounding box correctly.\n");
        return 0;
    } else {
        printf("TEST FAILED.\n");
        return 1;
    }
}

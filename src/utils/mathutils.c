#include "mathutils.h"

float rnd_normalized (void) {
    return (float)rand() / RAND_MAX;
}

int geometric_dist_test (float p, int max_val) {
    int x = 0;
    for (; x < max_val && rnd_normalized() <= p; x++);
    return x;
}
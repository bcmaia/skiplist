#ifndef MATHUTILS_H_INCLUDED
#define MATHUTILS_H_INCLUDED

#include <stdlib.h>

/**
 * @brief Returns a random float in the range [0, 1).
 * 
 * @return float 
*/
float rnd_normalized (void);

/**
 * @brief returns the number of times that the random function
 * rnd_normalized() returns a value less than or equal to p.
 * That number is bounded by max_val.
 * 
 * @param p the probability of success of the geometric distribution.
 * p = [0, 1)
 * @param max_val the maximum value that the geometric distribution.
 * @return int
 * 
*/
int geometric_dist_test (float p, int max_val);

#endif // MATHUTILS_H_INCLUDED
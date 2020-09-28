
#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "function.h"

typedef struct
{
    function_t f;

    /*
     * Triangle parameters
     */
    float dutycycle;
    float slope_rising;
    float slope_falling;

    // Derived:
    float angle_peak;
} triangle_t;


/**
 * Initialize a triangle function using a dutycycle
 */
void triangle_init_with_dutycycle(
        triangle_t*,
        float value_min,
        float value_max,
        float dutycycle,
        float frequency_triangle,
        float frequency_update
        );

/**
 * Initialize a triangle function using two slopes
 */
void triangle_init_with_slopes(
        triangle_t*,
        float value_start,
        float slope_rising,
        float slop_falling,
        float frequency_triangle,
        float frequency_update
        );

/**
 * Calculates, updates and returns the function's value
 * at the momentary step position
 */
float triangle_update(triangle_t*);


#endif

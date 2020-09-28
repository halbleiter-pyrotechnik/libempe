
#ifndef TRAPEZOID_H
#define TRAPEZOID_H

#include "function.h"

typedef struct
{
    function_t f;

    /*
     * Setup
     */
    float slope_rising;
    float slope_falling;

    /*
     * Derived
     */
    float angle_stop_rising;
    float angle_start_falling;
    float angle_stop_falling;
    float angle_restart_rising;

} trapezoid_t;


/**
 * Initialize a trapezoid function
 */
void trapezoid_init(
        trapezoid_t* trapezoid,
        float slope_rising,
        float slope_falling,
        float cutoff_min,
        float cutoff_max,
        float frequency_desired,
        float frequency_update
        );

/**
 * Update and return the function's value
 */
float trapezoid_update(trapezoid_t*);


#endif

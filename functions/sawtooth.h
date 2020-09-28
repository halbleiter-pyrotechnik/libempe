
#ifndef SAWTOOTH_H
#define SAWTOOTH_H


#include "function.h"

/**
 * A sawtooth is a function with a slope
 */
typedef struct
{
    function_t f;
    float slope;
} sawtooth_t;


/**
 * This calculates all sawtooth function parameters from the
 * desired frequency and the frequency of update function invokation.
 * If (value_start < value_stop) then the slope i.e. increment is positive (rising sawtooth),
 * otherwise the slope i.e. increment is negative (falling sawtooth).
 */
void sawtooth_init(
        sawtooth_t*,
        float value_start,
        float value_stop,
        float f_desired,
        float f_update
        );

/**
 * This function increments the sawtooth function's current value
 *
 * @return Momentary function value
 */
float sawtooth_update(sawtooth_t*);


#endif

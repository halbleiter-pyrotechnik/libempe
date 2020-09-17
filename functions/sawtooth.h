
#ifndef SAWTOOTH_H
#define SAWTOOTH_H


#include "function.h"

/**
 * A sawtooth is simply a function
 */
typedef function_t sawtooth_t;


#ifndef PI_x2
#define PI_x2 6.283185307179586f
#endif


/**
 * This calculates all sawtooth function parameters from the
 * desired frequency and the frequency of update function invokation.
 * If (value_start < value_stop) then the increment is positive (rising sawtooth),
 * otherwise the increment is negative (falling sawtooth).
 */
void sawtooth_init(
        sawtooth_t*,
        float value_start,
        float value_stop,
        float f_desired,
        float f_update
        );

/**
 * Sets a new sawtooth frequency and adjusts the stepsize accordingly
 */
void sawtooth_set_frequency(sawtooth_t*, float f_desired);

/**
 * This function increments the sawtooth function's current value
 *
 * @return Momentary function value
 */
float sawtooth_update(sawtooth_t*);


#endif

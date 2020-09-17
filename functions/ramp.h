
#ifndef RAMP_H
#define RAMP_H


#include "function.h"

/**
 * A ramp is simply a function
 */
typedef function_t ramp_t;


/**
 * Initialize a ramp function
 *
 * if (value_start > value_stop) then the increment will be positive (ramp goes upwards),
 * otherwise the increment will be negative (ramp goes downwards).
 */
void ramp_init(
        ramp_t*,
        float value_start,
        float value_stop,
        float ramp_up_time,
        float update_frequency
        );

/**
 * Update the given ramp by one step
 *
 * @return  The momentary value of the ramp
 */
float ramp_update(ramp_t*);


#endif

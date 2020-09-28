
#ifndef FUNCTION_H
#define FUNCTION_H

#include <stdbool.h>

#ifndef M_2x_PI
#define M_2x_PI 6.283185307179586
#endif

#ifndef M_3x_PI_2
#define M_3x_PI_2 4.71238898038469
#endif


/**
 * Function pointer to function value getter
 */
typedef float (*fp_get_value_by_angle_t)(void*, float);


/**
 * This struct stores all parameters for the generation
 * of an function generated through iterative updates.
 */
typedef struct
{
    /** The desired function frequency */
    float function_frequency;

    /** The frequency of update function invokation */
    float update_frequency;

    /** Number of periods to run; -1 = repeat indefinitely */
    int repetition_count;

    /**
     * Number of values per function period
     *
     * This value is derived from the provided
     * frequencies upon function initialization.
     */
    float step_count;

    /** Momentary step position, between 0 and step_count */
    float step;

    /** Momentary function argument x between 0.0 and 2*pi */
    float angle;

    /** By how much to increment x on each step */
    float angle_increment;

    /** The momentary function's output value */
    float value;

    /** The lowest possible value */
    float value_min;

    /** The largest possible value */
    float value_max;

    /** The difference between minimal and maximal value */
    float value_delta;

    /**
     * Pointer to a "method", that returns this function's value
     * given an angle between 0.0 and 2*PI (rad).
     */
    fp_get_value_by_angle_t* get_value_by_angle;

} function_t;


/**
 * Initialize a function
 */
void function_init(
        function_t*,
        float value_min,
        float value_max,
        float frequency_function,
        float frequency_update
        );

/**
 * Adjust the desired frequency of the function
 */
void function_set_frequency(
        function_t*,
        float frequency_function
        );

/**
 * Advance the function's x value to the next step position
 */
void function_step(function_t*);


#endif


#ifndef FUNCTION_H
#define FUNCTION_H


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

    /** The momentary function's output value */
    float value;

    /** The lowest possible value */
    float value_min;

    /** The largest possible value */
    float value_max;

    /** The difference between minimal and maximal value */
    float value_delta;

    /** Number of values per function period */
    float stepcount;

    /** By how much the output value is incremented on each function invokation */
    float value_increment;

} function_t;


#endif

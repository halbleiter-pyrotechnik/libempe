/**
 * This file declares types and functions to store
 * and sort values, which pop up during runtime,
 * such that they may be forwarded e.g. to analog debugger.
 */

#ifndef ANALOG_DEBUGGER_DEBUG_H
#define ANALOG_DEBUGGER_DEBUG_H

/*
 * The developer must have a file named analog-debugger.h in his/her project,
 * in which type debug_value_e is declared enumerating all the project-specific debug values,
 * see also: analog-debugger.h.template
 */
#include "analog-debugger.h"


typedef struct
{
    float value;
    float min;
    float max;
} debug_value_t;


/**
 * Initialize one debug value struct
 */
void debug_value_set_min_max(debug_value_e, float min, float max);

/**
 * Initialize an array of debug value structs
 */
void debug_values_set_min_max(debug_value_e*, uint32_t num, float min, float max);

/**
 * Store a value for debugging
 */
void set_debug_value(debug_value_e, float);

/**
 * Recall a value for debugging
 */
debug_value_t* get_debug_value(debug_value_e);

/**
 * Return a debug value
 */
float get_debug_value_immediate(debug_value_e);

/**
 * This function adjusts the configured min and max values,
 * if the measured value lies outside those boundaries.
 */
void debug_value_adjust_range(debug_value_t* value);

/**
 * This function limits the value to it's minimum or maximum,
 * if it lies outside those boundaries.
 */
void debug_value_clip_to_range(debug_value_t* value);


#endif

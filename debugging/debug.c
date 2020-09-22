
#include "debug.h"
#include "dac.h"
#include "../transformations/direct-quadrature.h"
#include <stdint.h>
#include <stdbool.h>


/**
 * This array stores all debug values
 */
static debug_value_t debug_values[DEBUG_VALUE_COUNT];


inline void debug_value_set_min_max(debug_value_e d, float min, float max)
{
    debug_value_t* v = get_debug_value(d);
    if (v != 0)
    {
        v->min = min;
        v->max = max;
    }
}


inline void debug_values_set_min_max(debug_value_e* v, uint32_t num, float min, float max)
{
    for (uint32_t i=0; i<num; i++)
    {
        debug_value_set_min_max(*v, min, max);
        v += 1;
    }
}


static inline bool is_valid_debug_value(debug_value_e selector)
{
    if (selector < DEBUGGING_DISABLED)
        return false;
    if (selector >= DEBUG_VALUE_COUNT)
        return false;
    return true;
}


inline void debug_value_adjust_range(debug_value_t* value)
{
    if (value->value < value->min)
        value->min = value->value;
    if (value->value > value->max)
        value->max = value->value;
}


inline void debug_value_clip_to_range(debug_value_t* value)
{
    if (value->value < value->min)
        value->value = value->min;
    if (value->value > value->max)
        value->value = value->max;
}


inline void set_debug_value(debug_value_e selector, float value)
{
    if (!is_valid_debug_value(selector))
        return;
    debug_value_t* v = get_debug_value(selector);
    v->value = value;
    debug_value_clip_to_range(v);
}


inline debug_value_t* get_debug_value(debug_value_e selector)
{
    if (!is_valid_debug_value(selector))
        return 0;
    return &(debug_values[selector]);
}


inline float get_debug_value_immediate(debug_value_e selector)
{
    debug_value_t* v = get_debug_value(selector);
    if (v == 0)
        return 0.0;
    return v->value;
}

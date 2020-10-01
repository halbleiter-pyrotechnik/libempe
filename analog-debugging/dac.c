
#include "dac.h"
#include "debug.h"
#include <stdint.h>
#include <stdbool.h>


/**
 * This array stores pointers to analog debug values,
 * which are to be transmitted to DACs.
 * The pointers are modified by select_value_for_debugging().
 */
static debug_value_t* dac_values[DAC_CHANNEL_COUNT] = {0};


inline void init_debugging_via_dac()
{
//    for (uint8_t i=0; i<4; i++)
//    {
//        // Null pointer
//        dac_values[i] = 0;
//    }
}


static inline bool is_valid_dac_channel(dac_channel_e channel)
{
    if (channel < DAC_CHANNEL1)
        return false;
    if (channel >= DAC_CHANNEL_COUNT)
        return false;
    return true;
}


void select_value_for_debugging(dac_channel_e channel, debug_value_e value_selector)
{
    if (!is_valid_dac_channel(channel))
        return;
    debug_value_t* v = get_debug_value(value_selector);
    if (v == 0)
        return;
    dac_values[channel] = v;
}


inline debug_value_t* get_dac_debug_value(dac_channel_e channel)
{
    if (!is_valid_dac_channel(channel))
        return 0;
    debug_value_t* v = dac_values[channel];
    return v;
}


/**
 * This function converts a float to a 8-bit DAC value
 */
static inline uint16_t convert_dac_value_to_raw(debug_value_t* debug_value)
{
    // Check if pointer is valid
    if (debug_value == 0)
        return 0;

    // We are using only 10 of the 12 avaliable bits on the DAC
    const uint16_t max_possible_value = 1023;

    // Prevent division by zero
    float divisor = (debug_value->max - debug_value->min);
    if (divisor == 0.0)
        return 0;

    return (uint16_t) (max_possible_value * (debug_value->value - debug_value->min) / divisor);
}


inline void write_debug_values_to_output(
        bool auto_adjust_min_max,
        raw_setpoints_t* raw_setpoints
        )
{
    if (auto_adjust_min_max)
    {
        for (uint8_t i=0; i<DAC_CHANNEL_COUNT; i++)
        {
            debug_value_adjust_range(dac_values[i]);
        }
    }

    for (uint8_t i=0; i<4; i++)
    {
#ifdef DAC_CHANNELS_REVERSE_ORDER
        raw_setpoints->value_dac[DAC_CHANNEL_COUNT-1-i] = convert_dac_value_to_raw(dac_values[i]);
#else
        raw_setpoints->value_dac[i] = convert_dac_value_to_raw(dac_values[i]);
#endif
    }
}


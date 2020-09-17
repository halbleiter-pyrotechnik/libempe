
#include "ramp.h"


void ramp_init(
        ramp_t* ramp,
        float value_start,
        float value_stop,
        float ramp_up_time,
        float update_frequency
        )
{
    ramp->value = value_start;

    if (value_stop > value_start)
    {
        // Ramp goes up
        ramp->value_min = value_start;
        ramp->value_max = value_stop;
    }
    else
    {
        // Ramp goes down
        ramp->value_max = value_start;
        ramp->value_min = value_stop;
    }

    ramp->update_frequency = update_frequency;

    if ((ramp_up_time == 0.0f) || (update_frequency == 0.0f))
    {
        /*
         * The ramp is being initialized with it's final values.
         */
        ramp->function_frequency = 0.0f;
        ramp->stepcount = 0.0f;
        ramp->value_delta = 0.0f;
        ramp->value_increment = 0.0f;
        ramp->value = value_stop;
        return;
    }

    ramp->function_frequency = 1.0f / ramp_up_time;
    ramp->stepcount = ramp_up_time * ramp->update_frequency;
    ramp->value_delta = value_stop - value_start;
    ramp->value_increment = ramp->value_delta / ramp->stepcount;
}


float ramp_update(ramp_t* ramp)
{
    if (ramp->value_increment != 0.0f)
    {
        // Advance one step
        ramp->value += ramp->value_increment;

        if (ramp->value_increment > 0.0)
        {
            // Ramp goes up
            if (ramp->value > ramp->value_max)
                ramp->value = ramp->value_max;
        }
        else if (ramp->value_increment < 0.0)
        {
            // Ramp goes down
            if (ramp->value < ramp->value_min)
                ramp->value = ramp->value_min;
        }
    }
    return ramp->value;
}

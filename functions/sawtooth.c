
#include "sawtooth.h"


void sawtooth_init(
        sawtooth_t* sawtooth,
        float value_start,
        float value_stop,
        float frequency_function,
        float frequency_update
        )
{
    function_init(
            &sawtooth->f,
            value_start,
            value_stop,
            frequency_function,
            frequency_update
            );

    /*
     * The value must reach it's maximum one increment before
     * the period is fully elapsed so that it can be zero again
     * in the following step.
     */
    sawtooth->slope = (value_stop - value_start) / (M_2x_PI - sawtooth->f.angle_increment);
}


float sawtooth_update(sawtooth_t* sawtooth)
{
    if (sawtooth->slope > 0.0)
    {
        // Rising sawtooth
        sawtooth->f.value = sawtooth->f.value_min + sawtooth->f.angle * sawtooth->slope;
        if (sawtooth->f.value > sawtooth->f.value_max)
            sawtooth->f.value = sawtooth->f.value_max;
    }
    else
    {
        // Falling sawtooth
        sawtooth->f.value = sawtooth->f.value_max + sawtooth->f.angle * sawtooth->slope;
        if (sawtooth->f.value < sawtooth->f.value_min)
            sawtooth->f.value = sawtooth->f.value_min;
    }

    return sawtooth->f.value;
}

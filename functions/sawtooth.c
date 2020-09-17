
#include "sawtooth.h"


void sawtooth_init(
        sawtooth_t* f,
        float value_start,
        float value_stop,
        float f_desired,
        float f_update
        )
{
    f->value_min = value_start;
    f->value_max = value_stop;
    f->value_delta = value_stop - value_start;
    f->update_frequency = f_update;
    sawtooth_set_frequency(f, f_desired);
}


void sawtooth_set_frequency(sawtooth_t* f, float f_desired)
{
    f->function_frequency = f_desired;

    if (f->function_frequency != 0.0)
        f->stepcount = f->update_frequency / f->function_frequency;
    else
        f->stepcount = 0.0;

    if (f->stepcount != 0.0)
        f->value_increment = f->value_delta / f->stepcount;
    else
        f->value_increment = 0.0;
}


float sawtooth_update(sawtooth_t* f)
{
    f->value += f->value_increment;

    if (f->value_increment > 0.0)
    {
        // Rising sawtooth
        if (f->value > f->value_max)
            f->value = f->value_min;
    }
    else if (f->value_increment < 0.0)
    {
        // Falling sawtooth
        if (f->value < f->value_min)
            f->value = f->value_max;
    }

    return f->value;
}

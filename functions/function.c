
#include "function.h"


void function_init(
        function_t* f,
        float value_min,
        float value_max,
        float frequency_function,
        float frequency_update
        )
{
    if (value_max < value_min)
    {
        float a = value_max;
        value_max = value_min;
        value_min = a;
    }

    f->value_min = value_min;
    f->value_max = value_max;
    f->value_delta = (value_max - value_min);
    f->value = value_min;

    f->update_frequency = frequency_update;
    function_set_frequency(f, frequency_function);

    f->step = 0.0;
    f->angle = 0.0;
    f->get_value_by_angle = 0;
    f->repetition_count = -1;
}


void function_set_frequency(
        function_t* f,
        float frequency_function
        )
{
    f->function_frequency = frequency_function;

    // Prevent division by zero:
    f->step_count = 1.0;
    if (f->function_frequency != 0.0)
        f->step_count = f->update_frequency / f->function_frequency;

    f->angle_increment = (1.0 / f->step_count) * M_2x_PI;
}


void function_step(function_t* f)
{
    if (f->repetition_count == 0)
    {
        // The number of configured function periods has expired.
        f->step = f->step_count;
        f->angle = M_2x_PI;
        return;
    }

    f->step += 1.0;
    while (f->step > f->step_count)
    {
        f->step -= f->step_count;
        if (f->repetition_count > 0)
            f->repetition_count--;
    }

    f->angle += f->angle_increment;
    while (f->angle < 0.0)
        f->angle += M_2x_PI;
    while (f->angle > M_2x_PI)
        f->angle -= M_2x_PI;
}


#include "trapezoid.h"
#include "math.h"
#include "stdlib.h"


void trapezoid_init(
        trapezoid_t* trapezoid,
        float slope_rising,
        float slope_falling,
        float cutoff_min,
        float cutoff_max,
        float frequency_function,
        float frequency_update
        )
{
    if (slope_rising < 0.0)
    {
        // The rising slope must be positive.
        slope_rising = -1.0 * slope_rising;
    }
    if (slope_falling > 0.0)
    {
        // The falling slope must be negative.
        slope_falling = -1.0 * slope_falling;
    }

    trapezoid->slope_rising = slope_rising;
    trapezoid->slope_falling = slope_falling;

    function_init(
            &trapezoid->f,
            cutoff_min,
            cutoff_max,
            frequency_function,
            frequency_update
            );

    trapezoid->angle_stop_rising = trapezoid->f.value_max / slope_rising;
    trapezoid->angle_start_falling = M_PI - trapezoid->angle_stop_rising;

    if (trapezoid->angle_start_falling < trapezoid->angle_stop_rising)
    {
        // The upper plateau is not reached.
        float angle_crossing = M_PI / (1.0 - (slope_rising / slope_falling));
        trapezoid->angle_stop_rising = angle_crossing;
        trapezoid->angle_start_falling = angle_crossing;
        trapezoid->f.value_max = slope_rising * angle_crossing;
        trapezoid->f.value_delta = trapezoid->f.value_max - trapezoid->f.value_min;
    }

    // TODO: There's something wrong here:
    float d = abs(trapezoid->f.value_delta / slope_falling) - trapezoid->angle_stop_rising;
    trapezoid->angle_stop_falling = M_PI - d;
    trapezoid->angle_restart_rising = M_2x_PI + d;

    if (trapezoid->angle_restart_rising < trapezoid->angle_stop_falling)
    {
        // The lower plateau is not reached.
        float angle_crossing = M_PI / (1.0 - (slope_falling / slope_rising));
        trapezoid->angle_stop_falling = M_PI + angle_crossing;
        trapezoid->angle_restart_rising = M_PI + angle_crossing;
        trapezoid->f.value_min = slope_falling * angle_crossing;
        trapezoid->f.value_delta = trapezoid->f.value_max - trapezoid->f.value_min;
    }
}


float trapezoid_update(trapezoid_t* trapezoid)
{
    if (trapezoid->f.angle <= trapezoid->angle_stop_rising)
    {
        trapezoid->f.value = trapezoid->slope_rising * trapezoid->f.angle;
    }
    else if (trapezoid->f.angle <= trapezoid->angle_start_falling)
    {
        trapezoid->f.value = trapezoid->f.value_max;
    }
    else if (trapezoid->f.angle <= trapezoid->angle_stop_falling)
    {
        trapezoid->f.value = trapezoid->f.value_max + trapezoid->slope_falling * (trapezoid->f.angle - trapezoid->angle_start_falling);
    }
    else if (trapezoid->f.angle <= trapezoid->angle_restart_rising)
    {
        trapezoid->f.value = trapezoid->f.value_min;
    }
    else
    {
        trapezoid->f.value = trapezoid->f.value_min + trapezoid->slope_rising * (trapezoid->f.angle - trapezoid->angle_restart_rising);
    }

    // TODO: For some reason the value can still underflow, making this necessary for the time being:
    if (trapezoid->f.value > trapezoid->f.value_max)
        trapezoid->f.value = trapezoid->f.value_max;
    if (trapezoid->f.value < trapezoid->f.value_min)
        trapezoid->f.value = trapezoid->f.value_min;

    return trapezoid->f.value;
}

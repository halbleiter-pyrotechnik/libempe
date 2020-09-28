
#include "triangle.h"


void triangle_init_with_dutycycle(
        triangle_t* triangle,
        float value_min,
        float value_max,
        float dutycycle,
        float frequency_triangle,
        float frequency_update
        )
{
    function_init(
            &triangle->f,
            value_min,
            value_max,
            frequency_triangle,
            frequency_update
            );

    if (dutycycle < 0.0)
        dutycycle = 0.0;
    if (dutycycle > 1.0)
        dutycycle = 1.0;
    triangle->dutycycle = dutycycle;
    triangle->angle_peak = dutycycle * M_2x_PI;
    triangle->slope_rising = triangle->f.value_delta / triangle->angle_peak;
    triangle->slope_falling = -triangle->f.value_delta / (M_2x_PI - triangle->angle_peak);
}


void triangle_init_with_slopes(
        triangle_t* triangle,
        float value_start,
        float slope_rising,
        float slope_falling,
        float frequency_triangle,
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

    triangle->slope_rising = slope_rising;
    triangle->slope_falling = slope_falling;

    // First we rise, then we fall
    triangle->dutycycle = 1.0 / (1.0 + slope_rising/slope_falling);
    triangle->angle_peak = triangle->dutycycle * M_2x_PI;

    function_init(
            &triangle->f,
            value_start,
            value_start + slope_rising * triangle->angle_peak,
            frequency_triangle,
            frequency_update
            );
}


float triangle_update(triangle_t* triangle)
{
    if (triangle->f.angle < triangle->angle_peak)
    {
        // Rising edge
        triangle->f.value = triangle->f.value_min + triangle->slope_rising * triangle->f.angle;
    }
    else
    {
        // Falling edge
        triangle->f.value = triangle->f.value_max + triangle->slope_falling * (triangle->f.angle - triangle->angle_peak);
    }

    return triangle->f.value;
}


#include "pwm_unit.h"
#include "math.h"

#define rad (M_PI / 180.0)

const float deadtime_hs_to_ls_correction = 6.75;
const float deadtime_ls_to_hs_correction = 3.5;

#define deadtime_min    75e-9
#define deadtime_max    300e-9
#define curve_height    (deadtime_max - deadtime_min)

#define angle_begin  28.0
#define angle_end    33.0
const float curve_factor = curve_height/((angle_begin*rad)*(angle_begin*rad)*(angle_begin*rad));

const float angle1 = angle_end * rad;
const float angle2 = (180.0 - angle_end) * rad;
const float angle3 = (180.0 + angle_end) * rad;
const float angle4 = (360.0 - angle_end) * rad;


#define array_length 360
static uint16_t deadtime_hs_to_ls[array_length] = {0};
static uint16_t deadtime_ls_to_hs[array_length] = {0};


void deadtime_lookup_table_init(
        pwm_unit_properties_t* pwm_unit
        )
{
    float angle = 0.0;
    const float angle_step = M_2x_PI / array_length;
    for (uint16_t i=0; i<array_length; i++)
    {
        get_deadtime_hs_to_ls_by_angle(pwm_unit, angle);
        get_deadtime_ls_to_hs_by_angle(pwm_unit, angle);
        angle += angle_step;
    }
}


static inline uint16_t get_deadtime_lookup_table_index(float angle)
{
    return angle / M_2x_PI * array_length;
}


inline uint16_t get_deadtime_hs_to_ls_by_angle(
        pwm_unit_properties_t* pwm_unit,
        float angle
        )
{
    uint16_t array_index = get_deadtime_lookup_table_index(angle);
    uint16_t deadtime = deadtime_hs_to_ls[array_index];
    if (deadtime != 0)
        return deadtime;

    float desired_deadtime = deadtime_min;

    if ((angle > M_PI) && (angle < angle3))
    {
        float a = (angle - M_PI);
        float angle_3 = a;
        angle_3 *= a;
        angle_3 *= a;
        desired_deadtime = deadtime_min + angle_3 * curve_factor;
    }
    else if (angle > angle4)
    {
        float a = (M_2x_PI - angle);
        float angle_3 = a;
        angle_3 *= a;
        angle_3 *= a;
        desired_deadtime = deadtime_min + angle_3 * curve_factor;
    }

    if (desired_deadtime < deadtime_min)
        desired_deadtime = deadtime_min;
    else if (desired_deadtime > deadtime_max)
        desired_deadtime = deadtime_max;

    /*
     * Measured:
     * 12 ticks = 75 ns
     * 24 ticks = 229 ns
     */
    uint16_t ticks = desired_deadtime * pwm_unit->tick_frequency + deadtime_hs_to_ls_correction;
    deadtime_hs_to_ls[array_index] = ticks;
    return ticks;
}


inline uint16_t get_deadtime_ls_to_hs_by_angle(
        pwm_unit_properties_t* pwm_unit,
        float angle
        )
{
    uint16_t array_index = get_deadtime_lookup_table_index(angle);
    uint16_t deadtime = deadtime_ls_to_hs[array_index];
    if (deadtime != 0)
        return deadtime;

    float desired_deadtime = deadtime_min;

    if (angle < angle1)
    {
        float angle_3 = angle;
        angle_3 *= angle;
        angle_3 *= angle;
        desired_deadtime = deadtime_min + angle_3 * curve_factor;
    }
    else if ((angle > angle2) && (angle < M_PI))
    {
        float a = (M_PI - angle);
        float angle_3 = a;
        angle_3 *= a;
        angle_3 *= a;
        desired_deadtime = deadtime_min + angle_3 * curve_factor;
    }

    if (desired_deadtime < deadtime_min)
        desired_deadtime = deadtime_min;
    else if (desired_deadtime > deadtime_max)
        desired_deadtime = deadtime_max;

    /*
     * Measured:
     * 12 ticks = 106ns
     * 24 ticks = 256ns
     */
    uint16_t ticks = desired_deadtime * pwm_unit->tick_frequency + deadtime_ls_to_hs_correction;
    deadtime_ls_to_hs[array_index] = ticks;
    return ticks;
}

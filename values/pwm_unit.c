
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


inline uint16_t get_deadtime_hs_to_ls_by_angle(
        pwm_unit_properties_t* pwm_unit,
        float angle
        )
{
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
    return ticks;
}


inline uint16_t get_deadtime_ls_to_hs_by_angle(
        pwm_unit_properties_t* pwm_unit,
        float angle
        )
{
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
    return ticks;
}

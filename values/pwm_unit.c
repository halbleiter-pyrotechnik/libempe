
#include "pwm_unit.h"
#include "math.h"

#define rad (M_PI / 180.0)

/*
 * Compensation for driver delays
 * when converting dead-times from nanoseconds to ticks
 */
const float deadtime_hs_to_ls_correction = 6.75;
const float deadtime_ls_to_hs_correction = 3.5;

/*
 * Upper and lower boundaries for dead-time durations (in nanoseconds)
 */
#define deadtime_min            75e-9
#define deadtime_max            300e-9
#define deadtime_default        75e-9

/*
 * Dead-time function parameters
 */
#define deadtime_curve_begin    120e-9
#define deadtime_curve_end      300e-9
#define deadtime_curve_height   (deadtime_curve_end - deadtime_curve_begin)

/** Phase angle at which the dead-time reaches it's maximum (choke current is low) */
#define angle_current_threshold     28.0
/** Phase angle at which the choke current has reached zero */
#define angle_current_zero          33.0

const float deadtime_curve_factor = deadtime_curve_height/((angle_current_threshold*rad)*(angle_current_threshold*rad)*(angle_current_threshold*rad));

const float angle1 = angle_current_zero * rad;
const float angle2 = (180.0 - angle_current_zero) * rad;
const float angle3 = (180.0 + angle_current_zero) * rad;
const float angle4 = (360.0 - angle_current_zero) * rad;

/*
 * Buffers to store pre-calculated dead-time values (in ticks)
 */
#define array_length 1120
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

    float desired_deadtime = deadtime_default;

    if ((angle > M_PI) && (angle < angle3))
    {
        float a = (angle - M_PI);
        float angle_3 = a;
        angle_3 *= a;
        angle_3 *= a;
        desired_deadtime = deadtime_curve_begin + angle_3 * deadtime_curve_factor;
    }
    else if (angle > angle4)
    {
        float a = (M_2x_PI - angle);
        float angle_3 = a;
        angle_3 *= a;
        angle_3 *= a;
        desired_deadtime = deadtime_curve_begin + angle_3 * deadtime_curve_factor;
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

    float desired_deadtime = deadtime_default;

    if (angle < angle1)
    {
        float angle_3 = angle;
        angle_3 *= angle;
        angle_3 *= angle;
        desired_deadtime = deadtime_curve_begin + angle_3 * deadtime_curve_factor;
    }
    else if ((angle > angle2) && (angle < M_PI))
    {
        float a = (M_PI - angle);
        float angle_3 = a;
        angle_3 *= a;
        angle_3 *= a;
        desired_deadtime = deadtime_curve_begin + angle_3 * deadtime_curve_factor;
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

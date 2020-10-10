
#include "pwm_unit.h"
#include "math.h"


void pwm_unit_init(
        pwm_unit_properties_t* pwm_unit
        )
{
    pwm_unit->flat_high_threshold = (pwm_unit->ticks_per_period-1) - pwm_unit->minimum_driver_off_time;

    /*
     * Calculate helper values
     */
    deadtime_parameters_t* deadtimes = pwm_unit->variable_deadtimes;
    deadtimes->deadtime_curve_height = (deadtimes->deadtime_curve_end - deadtimes->deadtime_curve_begin);
    float a = (deadtimes->angle_current_threshold*RAD);
    deadtimes->deadtime_curve_factor = deadtimes->deadtime_curve_height/(a*a*a);

    deadtimes->angle1 = deadtimes->angle_current_zero * RAD;
    deadtimes->angle2 = (180.0 - deadtimes->angle_current_zero) * RAD;
    deadtimes->angle3 = (180.0 + deadtimes->angle_current_zero) * RAD;
    deadtimes->angle4 = (360.0 - deadtimes->angle_current_zero) * RAD;

    /*
     * Pre-calculate all the dead-times
     */
    float angle = 0.0;
    const float angle_step = M_2x_PI / DEADTIME_LOOKUP_TABLE_LENGTH;
    for (uint16_t i=0; i<DEADTIME_LOOKUP_TABLE_LENGTH; i++)
    {
        get_deadtime_hs_to_ls_by_angle(pwm_unit, angle);
        get_deadtime_ls_to_hs_by_angle(pwm_unit, angle);
        angle += angle_step;
    }
}


inline uint16_t get_deadtime_lookup_table_index(float angle)
{
    return angle / M_2x_PI * DEADTIME_LOOKUP_TABLE_LENGTH;
}


inline uint16_t get_deadtime_hs_to_ls_by_angle(
        pwm_unit_properties_t* pwm_unit,
        float angle
        )
{
    uint16_t array_index = get_deadtime_lookup_table_index(angle);
    deadtime_parameters_t* deadtimes = pwm_unit->variable_deadtimes;
    if (deadtimes == 0)
        // Illegal pointer error
        return pwm_unit->deadtime_ticks_hs_to_ls;

    uint16_t deadtime = deadtimes->ticks_hs_to_ls[array_index];
    if (deadtime != 0)
        // Use pre-calculated dead-time
        return deadtime;

    float desired_deadtime;

    if (pwm_unit->use_variable_deadtimes)
    {
        if (angle > M_PI)
        {
            if (angle < deadtimes->angle3)
            {
                float a = (angle - M_PI);
                float angle_3 = a * a * a;
                desired_deadtime = deadtimes->deadtime_curve_begin + angle_3 * deadtimes->deadtime_curve_factor;
            }
            else if (angle > deadtimes->angle4)
            {
                float a = (M_2x_PI - angle);
                float angle_3 = a * a * a;
                desired_deadtime = deadtimes->deadtime_curve_begin + angle_3 * deadtimes->deadtime_curve_factor;
            }
            else
            {
                desired_deadtime = deadtimes->deadtime_curve_plateau;
            }
        }
        else
        {
            desired_deadtime = deadtimes->deadtime_outside_curve;
        }
    }
    else
    {
        desired_deadtime = pwm_unit->deadtime_hs_to_ls;
    }

    if (desired_deadtime < deadtimes->deadtime_min)
        desired_deadtime = deadtimes->deadtime_min;
    else if (desired_deadtime > deadtimes->deadtime_max)
        desired_deadtime = deadtimes->deadtime_max;

    /*
     * Measured:
     * 12 ticks = 75 ns
     * 24 ticks = 229 ns
     */
    deadtimes->deadtime_hs_to_ls[array_index] = desired_deadtime;
    uint16_t ticks = desired_deadtime * pwm_unit->tick_frequency + deadtimes->driver_correction_hs_to_ls;
    deadtimes->ticks_hs_to_ls[array_index] = ticks;
    return ticks;
}


inline uint16_t get_deadtime_ls_to_hs_by_angle(
        pwm_unit_properties_t* pwm_unit,
        float angle
        )
{
    uint16_t array_index = get_deadtime_lookup_table_index(angle);
    deadtime_parameters_t* deadtimes = pwm_unit->variable_deadtimes;
    if (deadtimes == 0)
        // Illegal pointer error
        return pwm_unit->deadtime_ticks_ls_to_hs;

    uint16_t deadtime = deadtimes->ticks_ls_to_hs[array_index];
    if (deadtime != 0)
        // Use pre-calculated dead-time
        return deadtime;

    float desired_deadtime;

    if (pwm_unit->use_variable_deadtimes)
    {
        if (angle < M_PI)
        {
            if (angle < deadtimes->angle1)
            {
                float angle_3 = angle * angle * angle;
                desired_deadtime = deadtimes->deadtime_curve_begin + angle_3 * deadtimes->deadtime_curve_factor;
            }
            else if (angle > deadtimes->angle2)
            {
                float a = (M_PI - angle);
                float angle_3 = a * a * a;
                desired_deadtime = deadtimes->deadtime_curve_begin + angle_3 * deadtimes->deadtime_curve_factor;
            }
            else
            {
                desired_deadtime = deadtimes->deadtime_curve_plateau;
            }
        }
        else
        {
            desired_deadtime = deadtimes->deadtime_outside_curve;
        }
    }
    else
    {
        desired_deadtime = pwm_unit->deadtime_ls_to_hs;
    }

    if (desired_deadtime < deadtimes->deadtime_min)
        desired_deadtime = deadtimes->deadtime_min;
    else if (desired_deadtime > deadtimes->deadtime_max)
        desired_deadtime = deadtimes->deadtime_max;

    /*
     * Measured:
     * 12 ticks = 106ns
     * 24 ticks = 256ns
     */
    deadtimes->deadtime_ls_to_hs[array_index] = desired_deadtime;
    uint16_t ticks = desired_deadtime * pwm_unit->tick_frequency + deadtimes->driver_correction_ls_to_hs;
    deadtimes->ticks_ls_to_hs[array_index] = ticks;
    return ticks;
}

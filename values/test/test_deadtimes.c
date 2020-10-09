
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "pwm_unit.h"


/*
 * Dead-time configuration example
 */
deadtime_parameters_t variable_deadtimes =
{
    driver_correction_hs_to_ls: 6.75,
    driver_correction_ls_to_hs: 3.5,

    deadtime_min: 75e-9,
    deadtime_max: 350e-9,

    deadtime_outside_curve: 120e-9,
    deadtime_curve_begin: 75e-9,
    deadtime_curve_end: 350e-9,
    deadtime_curve_plateau: 120e-9,

    angle_current_threshold: 22.0,
    angle_current_zero: 24.0,
};

/*
 * PWM unit configuration example
 */
pwm_unit_properties_t pwm_unit_properties =
{
    tick_frequency: 80e6,
    switching_frequency: 140e3,

    ticks_per_period: 571,
    ticks_max: 0x0fff,

    minimum_driver_on_time: 12,
    minimum_driver_off_time: 18,

    deadtime_hs_to_ls: 12,
    deadtime_ls_to_hs: 12,
    use_variable_deadtimes: true,
    variable_deadtimes: &variable_deadtimes,
};


void test_deadtimes(char* filename)
{
    FILE* f;
    f = fopen(filename, "w");

    pwm_unit_init(&pwm_unit_properties);
    deadtime_parameters_t* deadtimes = pwm_unit_properties.variable_deadtimes;

    const float step = 0.5;
    float angle_step = step * M_PI / 180.0;
    float angle = 0.0;
    for (uint32_t i=0; i<(uint32_t) (360 / step); i++)
    {
        uint16_t d1 = get_deadtime_hs_to_ls_by_angle(
                        &pwm_unit_properties,
                        angle
                        );
        uint16_t d2 = get_deadtime_ls_to_hs_by_angle(
                        &pwm_unit_properties,
                        angle
                        );
        uint16_t index = get_deadtime_lookup_table_index(angle);
        float f1 = deadtimes->deadtime_hs_to_ls[index] * 1e9;
        float f2 = deadtimes->deadtime_ls_to_hs[index] * 1e9;

        fprintf(f, "%d;%.2f;%d;%d;%.2f;%.2f;\n",
                i,
                angle,
                d1,
                d2,
                f1,
                f2
                );

        angle += angle_step;
    }

    fclose(f);
}

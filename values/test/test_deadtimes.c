
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "pwm_unit.h"


static pwm_unit_properties_t pwm_unit_properties =
{
    tick_frequency: 80e6,
    switching_frequency: 140e3,
    ticks_per_period: 571,
    ticks_max: 0x0fff,
    minimum_driver_on_time: 12,
    minimum_driver_off_time: 18,
    flat_high_threshold: (571-1) - 18,
};


void test_deadtimes(char* filename)
{
    FILE* f;
    f = fopen(filename, "w");

    deadtime_lookup_table_init(&pwm_unit_properties);

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

        fprintf(f, "%d;%.2f;%d;%d;\n",
                i,
                angle,
                d1,
                d2
                );

        angle += angle_step;
    }

    fclose(f);
}

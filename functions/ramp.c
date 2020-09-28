
#include "ramp.h"


void ramp_init(
        ramp_t* ramp,
        float value_start,
        float value_stop,
        float ramp_up_time,
        float update_frequency
        )
{
    sawtooth_init(
            ramp,
            value_start,
            value_stop,
            1/ramp_up_time,
            update_frequency
            );

    // Actually reach 2*pi when one period is elapsed
    ramp->f.angle_increment *= ramp->f.step_count / (ramp->f.step_count+1);
    // Run only once
    ramp->f.repetition_count = 1;
}


float ramp_update(ramp_t* ramp)
{
    return sawtooth_update(ramp);
}

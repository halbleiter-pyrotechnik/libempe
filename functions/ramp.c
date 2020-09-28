
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

    ramp->f.repetition_count = 1;
}


float ramp_update(ramp_t* ramp)
{
    return sawtooth_update(ramp);
}


#include "sine.h"
#include "stdint.h"
#include "math.h"


void sine_modulate_ccm(
        float phase_angle_rad,
        float amplitude,
        modulation_values_ccm_t* modulation
        )
{
    for (uint8_t i=0; i<3; i++)
    {
        modulation->value[i] = amplitude * sinf(phase_angle_rad);
        phase_angle_rad += RAD_120_DEGREE;
    }
}

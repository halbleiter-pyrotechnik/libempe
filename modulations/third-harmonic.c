
#include "third-harmonic.h"
#include "math.h"


void apply_third_harmonic_ccm(
        float modulation_amplitude,
        modulation_values_ccm_t* modulation,
        float phase_angle
        )
{
    //const float overmodulation_factor = 2.0f / M_SQRT3;
    //const float third_harmonic_factor = overmodulation_factor - 1.0f;
    const float overmodulation_factor = 1.1547f;
    const float third_harmonic_factor = 1.0/6.0; //0.1547f;

    float third_harmonic_value = modulation_amplitude * third_harmonic_factor * sinf(M_PI + (3.0f * phase_angle));

    for (uint8_t i=0; i<3; i++)
    {
        modulation->value[i] *= overmodulation_factor;
        modulation->value[i] -= third_harmonic_value;
    }
}

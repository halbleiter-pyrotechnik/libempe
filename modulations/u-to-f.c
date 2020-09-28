
#include "u-to-f.h"
#include "sine.h"
#include <stdint.h>
#include <math.h>


void uf_control_init(
        uf_control_t* uf,
        uf_control_init_t* init
        )
{
    /*
     * Initialize a sawtooth function for the phase angle
     */
    sawtooth_init(
            &uf->sine_angle,
            0.0,
            M_2x_PI,
            init->desired_frequency,
            init->update_frequency
            );

    /*
     * Typically one wouldn't start the motor with zero voltage,
     * but at about 10% of the nominal motor voltage.
     */
    float startup_voltage = 0.1 * init->desired_voltage_rms;
    ramp_init(
            &uf->voltage_rms,
            startup_voltage,
            init->desired_voltage_rms,
            init->ramp_up_duration,
            init->update_frequency
            );

    float startup_frequency = 0.0;
    ramp_init(
            &uf->frequency,
            startup_frequency,
            init->desired_frequency,
            init->ramp_up_duration,
            init->update_frequency
            );

    float startup_modulation_factor = 0.1f;
    ramp_init(
            &uf->modulation_factor,
            startup_modulation_factor,
            init->desired_modulation_factor,
            init->ramp_up_duration,
            init->update_frequency
            );
}


inline float uf_calculate_modulation_factor(
        float desired_threephase_voltage,
        float dclink_voltage
        )
{
    return (desired_threephase_voltage * SQRT2_x2) / dclink_voltage;
}


float uf_control_run(
        uf_control_t* uf,
        float dclink_voltage,
        modulation_values_ccm_t* setpoints
        )
{
    /*
     * Advance phase angle, voltage ramp and frequency ramp by one step each
     */
    ramp_update(&uf->frequency);
    ramp_update(&uf->voltage_rms);
    function_set_frequency(&uf->sine_angle.f, uf->frequency.f.value);
    sawtooth_update(&uf->sine_angle);

    /*
     * Calculate modulation factor
     */
    float M;
    if (uf->update_modulation_factor_from_dclink_voltage)
    {
         M = uf_calculate_modulation_factor(
                uf->voltage_rms.f.value,
                dclink_voltage
                );
        uf->modulation_factor.f.value = M;
    }
    else
    {
        ramp_update(&uf->modulation_factor);
        M = uf->modulation_factor.f.value;
    }

    /*
     * Overmodulation is permissible when using flat-top
     * or applying third-harmonic modulation.
     */
    const float permissible_overmodulation = 2 / M_SQRT3;
    if (M > permissible_overmodulation)
        M = permissible_overmodulation;
    if (M < 0.0f)
        M = 0.0f;

    /*
     * Calculate the required duty cycles (phase-shifted by 120 degrees)
     *
     * Realize continous current mode (CCM):
     * Highside on-time + Lowside on-time = 1.0
     */
    sine_modulate_ccm(uf->sine_angle.f.value, M, setpoints);

    return M;
}


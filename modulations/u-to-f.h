/**
 * U/f is the simplest variant of a frequency inverter.
 * Motor voltage and frequency are controlled such that
 * they maintain a constant ratio towards one another.
 */


#ifndef UF_CONTROL_H
#define UF_CONTROL_H


#include <stdint.h>
#include <stdbool.h>
#include "../functions/ramp.h"
#include "../functions/sawtooth.h"
#include "modulation.h"


#ifndef PI_x2
#define M_2x_PI 6.283185307179586f
#endif

#ifndef SQRT2
#define SQRT2 1.4142135623730951f
#endif

#ifndef SQRT2_x2
#define SQRT2_x2 2.8284271247461903f
#endif


/**
 * This struct stores all parameters required for U/f control.
 */
typedef struct
{
    /**
     * The momentary phase angle between 0 and 2x pi
     * is incremented once every time the u-to-f function is run.
     */
    sawtooth_t sine_angle;

    /**
     * A ramp for the effective motor voltage (RMS)
     */
    ramp_t voltage_rms;

    /**
     * A ramp for the motor frequency
     */
    ramp_t frequency;

    /**
     * Modulation amplitude
     */
    ramp_t modulation_factor;

    /**
     * The modulation factor above can either simply be ramped up
     * or be calculated on each iteration from the DC link voltage
     * and the desired effective output voltage.
     */
    bool update_modulation_factor_from_dclink_voltage;

} uf_control_t;


/**
 * All parameters required to initialize a U/f modulation struct
 */
typedef struct
{
    float desired_frequency; // Hz
    float update_frequency; // Hz
    float desired_modulation_factor; // 0.0f - 1.0f
    float desired_voltage_rms; // Volts
    bool  update_modulation_factor_from_dclink_voltage;
    float ramp_up_duration; // seconds
} uf_control_init_t;


/**
 * Initialize U/f modulation
 */
void uf_control_init(
        uf_control_t*,
        uf_control_init_t*
        );

/**
 * Calculate the PWM modulation factor required
 * to achieve the desired RMS voltage at the motor
 * when given the voltage of a DC link capacitor.
 */
float uf_calculate_modulation_factor(
        float desired_threephase_voltage,
        float dclink_voltage
        );

/**
 * Run the U/f control once,
 * update phase angle, momentary voltage, momentary frequenc
 * and calculate the dutycycles required to achieve
 * the desired output voltage and frequency accordingly.
 *
 * @return  Modulation amplitude factor M
 */
float uf_control_run(
        uf_control_t*,
        float dclink_voltage,
        modulation_values_ccm_t*
        );


#endif

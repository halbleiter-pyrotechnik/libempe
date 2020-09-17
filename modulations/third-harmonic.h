
#ifndef THIRD_HARMONIC_H
#define THIRD_HARMONIC_H

#include "modulation.h"

/**
 * This adds a third harmonic sine wave to the modulation
 */
void apply_third_harmonic_ccm(
        float modulation_amplitude,
        modulation_values_ccm_t*,
        float phase_angle
        );

#endif
